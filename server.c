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

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno, clilen, n;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	if(argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) 
		error("ERROR opening socket");

	/* fill in port number to listen on. IP address can be anything (INADDR_ANY) */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* bind socket to this port number on this machine */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	
	/* listen for incoming connection requests */
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	printf("Listening on port %d\n", portno);

	/* accept a new request, create a newsockfd */
	while(newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)){
		if(newsockfd < 0) error("ERROR on accept");
		else printf("Connection accepted\n");

		/* read message from client */
		bzero(buffer,256);
		while(n = read(newsockfd,buffer,255)){
			if(n < 0) error("ERROR reading from socket");
			else printf("Here is the message: %s\n", buffer);
		}
	}

	return 0; 
}

