#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define MAX_MSG_LENGTH 100
#define MAX_CLIENTS 2

char buffer[MAX_MSG_LENGTH];

void error(char* msg, int retVal){
    perror(msg);
    exit(retVal);
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Usage: %s port number",argv[0]);
        return 1;
    }

    int sockfd, portno, clients[MAX_CLIENTS], client_count = 0;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    portno = htons(atoi(argv[1]));

    //set up the socket with TCP protocol over the internet
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("Couldn't create the socket!",2);
    
    //we use sockaddr_in struct because it is recommended to use for TCP/IP protocol
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = portno;
    client_addr_len = sizeof(client_addr);

    if(bind(sockfd,(struct sockaddr*) &server_addr, sizeof(server_addr)) != 0){
        error("Couldn't bind the socket to the port.",3);
    }

    //listen for 2 connections
    if(listen(sockfd,MAX_CLIENTS) != 0){
        error("Can't listen.",4);
    }

    //first we wait for player 1 to join
    clients[client_count] = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_len);
    printf("player %d has joined!",client_count+1);

    




    //close connections
    for(int i = 0; i < MAX_CLIENTS; i++){
        close(clients[i]);
    }
    close(sockfd);
    return EXIT_SUCCESS;
}