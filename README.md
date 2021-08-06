# container-from-scratch

This project builds a simple container from scratch using Linux namespaces and cgroups, with the intention of learning some of the low-level details about containers and virtualization in general. 

### Overview
*Often thought of as cheap VMs, containers are just isolated groups of processes running on a single host ([Eric Chiang](https://ericchiang.github.io/post/containers-from-scratch/))*. This isolation is produced using two Linux features - namespaces and cgroups. Namespaces partition kernel resources such that one set of processes sees one set of resources while another set of processes sees a different set of resources. Cgroups allow programmers to impose resource constraints on processes. You can read more [here](https://www.cse.iitb.ac.in/~cs695/slides_pdf/11-containers.pdf).

### Setting things up
First, you'll need a root filesystem for the container. Download one from [here](https://github.com/ericchiang/containers-from-scratch/releases/download/v0.1.0/rootfs.tar.gz). Untar it and place it in the root directory. Next, create two folders which will be linked by container.cpp, this allows for communication between the container and the external environment. 
```
$ mkdir rootfs/var/shared_folder/
$ mkdir shared_folder/
```

Next, compile the programs (the server and the hungry program will be run from within the container, hence we generate the executables in the shared folder.
```
$ g++ container.cpp -o container
$ gcc client.c -o client
$ gcc server.c -o shared_folder/server
$ g++ hungry.cpp -o shared_folder/hungry
```

Install `ifconfig` using `sudo apt-get install net-tools` if your system doesn't have it installed already.

### Running the code
Start the container instance (`<ip_in>` later belongs the ip address of the server which runs inside the container)
```
$ sudo ./container <hostname> <ip_in> <ip_out>
```
You may run commands like `ls` and `ps` to observe that the process you are does have isolation. 

There is network connectivity between the container's namespace and the external namespace. You can run a server inside the container as `$ cd var/shared_folder && ./server <port_number>` and a client in the parent as `$ ./client <ip_in> <port_number>)` to see some communication.

You can run the hungry program in the container as `$ cd vaqr/shared_folder && ./hungry`. The program requests a lot of memory from the kernel, and after a while is denied and killed becuase the container was initialized with a memory restriction (using cgroups). 


This project was done for a course assignment of [CS 695: Virtualization and Cloud Computing](https://www.cse.iitb.ac.in/~cs695/index.html) which I did in my third year at IITB. Details about the assignment details can be found [here](https://www.cse.iitb.ac.in/~cs695/pa/pa3.html). Much of this project is based on [this](https://cesarvr.github.io/post/2018-05-22-create-containers/) blog post. [This](https://ericchiang.github.io/post/containers-from-scratch/) was also a useful resource.  
