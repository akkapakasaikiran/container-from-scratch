#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>
#include <signal.h>

void error(char *msg){
	perror(msg);
	exit(1);
}

int create_socket_and_connect(char* ip, int portno, struct sockaddr_in serv_addr){
	/* create socket, get sockfd handle */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) 
		error("ERROR opening socket");

	/* fill in server address in sockaddr_in datastructure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(portno);

	/* connect to server */
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	
	return sockfd;
}

void send_message_to_server(int sockfd, char* ip){
	char* str = "Hello, server!";
	write(sockfd, str, strlen(str));
	printf("Message sent to IP %s \n", ip);
}

int main(int argc, char *argv[]){
	if(argc != 3)
		error("Error: Usage: ./client server_ip port\n");

	struct sockaddr_in serv_addr;
	char* ip = argv[1];
	int portno = atoi(argv[2]);
	int sockfd = create_socket_and_connect(ip, portno, serv_addr);

	while(1){
		sleep(1);
		send_message_to_server(sockfd, ip);
	}

	return 0;
}
