#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include<string.h>

#define MAX_CMD_LENGTH 100
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100
#define EXTRA 1024

static char buffer[MAX_CMD_LENGTH];
static char* tokens[MAX_TOKEN_LENGTH];
static char extraBuffer[EXTRA];


//Parses command found in Buffer. Sends tokens to tokens: array of strings
void parseCommand(){
    int num_tokens = 0;
    int i = 0;
    int current_token_length = 0;
    int in_token = 0;

    while(i < MAX_CMD_LENGTH && buffer[i] != '\0'){
        if(buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|' || buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\t'){
            if(in_token){
                tokens[num_tokens][current_token_length] = '\0';
                num_tokens++;
                current_token_length = 0;
                in_token = 0;
            }
            if(buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|'){
            tokens[num_tokens] = malloc(4*sizeof(char));
            tokens[num_tokens][0] = buffer[i];
            tokens[num_tokens][1] = '\0';
            num_tokens++;
            }
        }
        else{
            if(!in_token){
                tokens[num_tokens] = malloc(MAX_TOKEN_LENGTH*sizeof(char));
                in_token = 1;
            }
            tokens[num_tokens][current_token_length] = buffer[i];
            current_token_length++;
        }
        i++;
    }

    if(in_token){
        tokens[num_tokens][current_token_length] = '\0';
        num_tokens++;
    }
}

//checks for null terminator in buffer
int getSizeOfCurrCmd(){
    for(int i = 0;i < MAX_CMD_LENGTH;i++){
        if(buffer[i] == '\0'){
            return i;
        }
    }
    return MAX_CMD_LENGTH;
}

//Prints next line in interactive mode. Should recognize if previous command caused an error and add !
void printNextLine(){
    memset(buffer,0,MAX_CMD_LENGTH);
    strcpy(buffer,"mysh> ");
    write(STDOUT_FILENO,buffer,getSizeOfCurrCmd());
    memset(buffer,0,MAX_CMD_LENGTH);
}

//Helper method to print tokens after parsing command
void printCmds(){
    int i = 0;
    while(i < MAX_TOKEN_LENGTH && tokens[i] != NULL){
        printf("%s\n",tokens[i]);
        i++;
    }
}

//Checks if the input command is longer than the buffer by setting fd to non-blocking mode and
//trying to read any extra input.
int CheckForExtraInput(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    ssize_t extra_bytes_read = read(fd,extraBuffer,EXTRA);
    fcntl(fd,F_SETFL,flags);
    if(extra_bytes_read != -1){
        return 1;
    }
    memset(extraBuffer,0,EXTRA);
    return 0;
}

void processCommand(){
    char* cmd = tokens[0];
    if(strcmp(cmd,"cd") == 0){
        //complete cd process
    }
    else if(strcmp(cmd,"pwd") == 0){

    }
    else if(strcmp(cmd,"ls") == 0){

    }
    else if(strcmp(cmd,"mkdir") == 0){

    }
    else if(strcmp(cmd,"cat")){

    }
    //add more commands that we can process
}


void CommandLoop(int fd){
    ssize_t bytes_read;
    while(1){
        bytes_read = read(fd,buffer,MAX_CMD_LENGTH);
        if(bytes_read == -1 || CheckForExtraInput(fd) == 1){
            perror("Error with Reading command. Try again\n");
            printNextLine();
            continue;
        }
        else{
            if(strcmp(buffer,"exit\n") == 0){break;}
            parseCommand();
            printNextLine();
        }
    }
}

void InteractiveShell(){
    strcpy(buffer,"Welcome to my Terminal\n");
    write(STDOUT_FILENO,buffer,getSizeOfCurrCmd());
    printNextLine();
    
    CommandLoop(STDIN_FILENO);

}

void BatchShell(int fd){

}


int main(int argc, char* argv[]){
    if(argc > 1){
        int fd = open(argv[1], O_RDONLY);

        if(fd == -1){
            printf("Error with opening file argument");
            return EXIT_FAILURE;
        }

        BatchShell(fd);
    }
    InteractiveShell();

    return EXIT_SUCCESS;

}