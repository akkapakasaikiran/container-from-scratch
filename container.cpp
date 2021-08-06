#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define CGROUP_FOLDER1 "/sys/fs/cgroup/memory/container/"
#define CGROUP_FOLDER2 "/sys/fs/cgroup/pids/container/"
#define concat(a,b) (a"" b)


char* stack_memory(){
	const int stackSize = 65536;
	auto *stack = new (std::nothrow) char[stackSize];
	if(stack == nullptr){ printf("Cannot allocate memory \n"); exit(1); }  

	// return end pointer of the array because the stack grows backward
	return stack + stackSize;   
}	

int run(const char *name){	
	char *_args[] = {(char *)name, (char *)0 };
	execvp(name, _args);
}

void write_to_file(const char* path, const char* val){
	int fd = open(path, O_WRONLY | O_APPEND);
	write(fd, val, strlen(val));
	close(fd);
}

void limit_memory(){
	mkdir(CGROUP_FOLDER1, S_IRUSR | S_IWUSR);
	const char* pid  = std::to_string(getpid()).c_str();
	
	write_to_file(concat(CGROUP_FOLDER1, "cgroup.procs"), pid);
	write_to_file(concat(CGROUP_FOLDER1, "notify_on_release"), "1");
	write_to_file(concat(CGROUP_FOLDER1, "memory.limit_in_bytes"), "10000000");
}

void limit_procs(){
	mkdir(CGROUP_FOLDER2, S_IRUSR | S_IWUSR);
	const char* pid  = std::to_string(getpid()).c_str();

	write_to_file(concat(CGROUP_FOLDER2, "cgroup.procs"), pid);
	write_to_file(concat(CGROUP_FOLDER2, "notify_on_release"), "1");
	write_to_file(concat(CGROUP_FOLDER2, "pids.max"), "8");
}

struct clone_arg {
	const char* hostname; 
	int veth_in;
	int ip_in;	
};

int jail(void* arg){
	sleep(1);
	struct clone_arg* jarg = (struct clone_arg*)arg;
	sethostname(jarg->hostname, strlen(jarg->hostname));

	char buf[256];
	sprintf(buf, "ifconfig veth%d 10.0.0.%d", jarg->veth_in, jarg->ip_in);
	system(buf); 

	limit_memory();
	limit_procs();

	clearenv();
	chroot("rootfs");
	chdir("/");
	mount("proc", "/proc", "proc", 0, 0);	// mount(source, target, type, ...);

	auto runThis = [](void *args) -> int { run("/bin/bash"); };
	clone(runThis, stack_memory(), SIGCHLD, nullptr);
	wait(nullptr);

	umount("/proc");
	return 0;
}


int main(int argc, char* argv[]){
	if(argc != 4){
		std::cerr<<"Usage: ./container <hostname> <ip_in> <ip_out>"<<std::endl;
		exit(1);
	}

	system("sudo mount --bind -o ro $PWD/shared_folder $PWD/rootfs/var/shared_folder");

	char buf[256];
	srand(time(0));
	int veth_in = rand(), veth_out = rand();
	int ip_in = atoi(argv[2]), ip_out = atoi(argv[3]);

	struct clone_arg marg;
	marg.hostname = argv[1]; marg.veth_in = veth_in; marg.ip_in = ip_in;

	int namespaces = CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNET;
	pid_t pid = clone(jail, stack_memory(), namespaces | SIGCHLD, (void*)&marg);

	sprintf(buf, "ip link add name veth%d type veth peer name veth%d netns %d", veth_out, veth_in, pid);
	system(buf);
	sprintf(buf, "ifconfig veth%d 10.0.0.%d", veth_out, ip_out);
	system(buf);

	wait(nullptr);

	system("sudo umount $PWD/rootfs/var/shared_folder");

	return 0;
}