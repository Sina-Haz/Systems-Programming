#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "message.h"
#include "message.c"

#define MAX_PLAYERS 252
#define GAME_PARTICIPANTS 2
#define BOARD_SIZE 9

int player_count = 0;
pthread_mutex_t mutexcount;
int movesPlayed = 0;
int lastIndex = 0;
char* curr_msg;

//initialize for new game
void setMovesToBeggining(){
    movesPlayed = 0;
    lastIndex = 0;
}

void error(const char *msg)
{
    perror(msg);
    pthread_exit(NULL);
}

//function that returns a socket fd for a socket bound to specified port
int setup_socket(int portno){
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){error("Couldn't create socket!");}

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR binding listener socket.");
    
    return sockfd;
}

void write_msg_to_player(int playerfd,char* msg){
    signal(SIGPIPE, SIG_IGN);
    int n = write(playerfd, msg, strlen(msg));
    if (n < 0)
        error("ERROR writing msg to client socket");
    
}

//this is a function for identifying what the command is (only recognizes commands players can send)
int getPlayerCommand(char* cmd){
    int num = -1;
    if(strcmp(cmd,"PLAY") == 0){
        num = 1;
    }else if(strcmp(cmd,"MOVE") == 0){
        num = 2;
    }else if(strcmp(cmd,"RSGN") == 0){
        num = 3;
    }else if(strcmp(cmd,"DRAW") == 0){
        num = 4;
    }
    return num;
}

//In this function we are: 
//1) Reading a full message. If a malformed or errorful message is sent we will terminate.
// If missing info sent we keep reading until we get a full msg.
// If the msg is overflowing we will only use 1st message and hold the rest of the message in the buffer
//numbers: -1 -> invalid command, 1 -> PLAY, 2 -> MOVE, 3 -> RSGN, 4 -> DRAW 
int recv_msg(int cli_sockfd)
{
    signal(SIGPIPE, SIG_IGN);
    int num,read_bytes,overflowed;
    
    //keep track of whether there was an overflow message or nah
    overflowed = 0;

    //step 1: check if there's anything in the message buffer in the first place
    if(strlen(msg_buf) == 0){
        read_bytes = read(cli_sockfd,msg_buf,MSG_LEN);
        num = identify_msg(read_bytes);
        //Server will terminate if there's any issue with the message
        if(num <= 0){
            exit(num);
        }
    }else{
        read_bytes = strlen(msg_buf);
        num = identify_msg(read_bytes);
        if(num <= 0){
            exit(num);
        }
    }

    while(num != PROPER_MSG){
        if(num == MISSING_INFO){
            int addl_bytes = read(cli_sockfd,msg_buf+read_bytes,MSG_LEN-read_bytes);
            if(addl_bytes == 0 || addl_bytes == -1){exit(addl_bytes);}
            read_bytes += addl_bytes;
            num = identify_msg(read_bytes);
        }
        if(num == OVERFLOW_MSG){
            num = handle_overflow();
            overflowed = 1;
        }
        if(num <= 0){exit(num);}
    }

    free(curr_msg);
    //now we should have a proper message either in msg_buf or first_msg depending on overflow
    if(overflowed == 1){
        get_msg_tokens(first_msg);
        curr_msg = strdup(first_msg);
    }else{
        get_msg_tokens(msg_buf);
        curr_msg = strdup(msg_buf);
    }

    char* cmd = msg_fields[0];


    return getPlayerCommand(cmd);
}

//This function setups listeners for the amount of players and accepts a batch of two new players 
void get_players(int socket, int* player_fd){

    socklen_t clilen;
    struct sockaddr_in cli_addr;

    int num_connections = 0;
    while(num_connections < GAME_PARTICIPANTS){
        //setup a listener to listen at that socket for number connections which is MAX_CONNECTIONS - curr_players
        if(listen(socket, MAX_PLAYERS + 1 - player_count) != 0){error("Listen error");}
        memset(&cli_addr, 0, sizeof(cli_addr));
        clilen = sizeof(cli_addr);

        player_fd[num_connections] = accept(socket,(struct sockaddr*) &cli_addr,&clilen);
        
        if (player_fd[num_connections] < 0)
            error("ERROR accepting a connection from a client.");
        
        //give player an ID so they know if they are player 0 or 1
        write(player_fd[num_connections], &num_connections, sizeof(int));

        //increment the number of players on the server
        pthread_mutex_lock(&mutexcount);
        player_count++;
        printf("Number of players is now %d.\n", player_count);
        pthread_mutex_unlock(&mutexcount);

        if(num_connections == 0){write_msg_to_player(player_fd[num_connections], "wait for other player to join.");}

        num_connections++;
    }
}

//helper method to write a BEGN msg to the player. id = 0 -> player uses X otherwise use O.
void write_begn_msg(int playerfd,char* player_name,int id){

    char* msg = malloc(sizeof(char)*MSG_LEN);
    bzero(msg,MSG_LEN);

    strcat(msg,"BEGN|");

    char num_str[3];
    sprintf(num_str,"%lu",strlen(player_name)+3);

    strcat(msg,num_str);
    strcat(msg,"|");

    if(id == 0){strcat(msg,"X|");}
    else if(id == 1){strcat(msg,"O|");}

    strcat(msg,player_name);
    strcat(msg,"|");

    write_msg_to_player(playerfd,msg);
    free(msg);
}

//this is the function that the threads will run. Parameter thread_data will be the two player fds for the game
void *run_game(void* thread_data){
    int* player_fd = (int*) thread_data;
    char* player_names[GAME_PARTICIPANTS];
    char board[BOARD_SIZE] = ".........";

    printf("Starting Game!\n");

    //now we wait for first player to send PLAY message
    if(recv_msg(player_fd[0]) != 1)
        error("Player 1 didn't start game properly.");
    
    player_names[0] = strdup(msg_fields[2]);

    write_msg_to_player(player_fd[0],"WAIT|0|");

    if(recv_msg(player_fd[1]) != 1)
        error("Player 2 didn't start game properly");

    player_names[1] = strdup(msg_fields[2]);

    write_msg_to_player(player_fd[1],"WAIT|0|");

    //write begin messages to players
    write_begn_msg(player_fd[0],player_names[0],0);
    write_begn_msg(player_fd[1],player_names[1],1);

    int prev_player_turn = 1;
    int player_turn = 0;
    int game_over = 0;
    int turn_count = 0;
    while(!game_over){

        int valid = 0;
        int move = 0;
        while(!valid){
            
        }
    }


    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    setMovesToBeggining();

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    int listen_sockfd = setup_socket(atoi(argv[1]));

    //using pthread_mutex_init we create a mutex object with default attributes.
    //mutexes are sync objects we use so that certain shared thread resources can be accessed only 1 at a time
    pthread_mutex_init(&mutexcount, NULL);

    while(1){
        if(player_count <= MAX_PLAYERS){
            //setup the fds from our 2 players.
            int* player_fd = malloc(sizeof(int)*GAME_PARTICIPANTS);
            memset(player_fd, 0,GAME_PARTICIPANTS * sizeof(int));

            //setup listener and accept a batch of two players.
            get_players(listen_sockfd,player_fd);

            //create a thread that will run the game for these two players.
            pthread_t thread;
            int result = pthread_create(&thread, NULL, run_game, (void *)player_fd);
            if (result)
            {
                printf("Thread creation failed with return code %d\n", result);
                exit(-1);
            }
        }
    }
    
}
































//Prev stuff:
// #include<stdio.h>
// #include<stdlib.h>
// #include<unistd.h>
// #include<string.h>
// #include<sys/types.h>
// #include<sys/socket.h>
// #include<netinet/in.h>

// #define MAX_MSG_LENGTH 100
// #define MAX_CLIENTS 2

// char buffer[MAX_MSG_LENGTH];

// void error(char* msg, int retVal){
//     perror(msg);
//     exit(retVal);
// }

// int main(int argc, char* argv[]){
//     if(argc < 2){
//         printf("Usage: %s port number",argv[0]);
//         return 1;
//     }

//     int sockfd, portno, clients[MAX_CLIENTS], client_count = 0;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_addr_len;

//     portno = htons(atoi(argv[1]));

//     //set up the socket with TCP protocol over the internet
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if(sockfd < 0)
//         error("Couldn't create the socket!",2);
    
//     //we use sockaddr_in struct because it is recommended to use for TCP/IP protocol
//     bzero(&server_addr,sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = portno;
//     client_addr_len = sizeof(client_addr);

//     if(bind(sockfd,(struct sockaddr*) &server_addr, sizeof(server_addr)) != 0){
//         error("Couldn't bind the socket to the port.",3);
//     }

//     //listen for 2 connections
//     if(listen(sockfd,MAX_CLIENTS) != 0){
//         error("Can't listen.",4);
//     }

//     //first we wait for player 1 to join
//     clients[client_count] = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_len);
//     printf("player %d has joined!",client_count+1);

    




//     //close connections
//     for(int i = 0; i < MAX_CLIENTS; i++){
//         close(clients[i]);
//     }
//     close(sockfd);
//     return EXIT_SUCCESS;
// }