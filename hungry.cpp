#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <fcntl.h>

int main(){
	for(int j = 0; j < 100000000; j++)
		int* a = new int;

	std::cout<<"I am done"<<std::endl;
}