#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include<string.h>

#define MAX_CMD_LENGTH 100
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100

static char buffer[MAX_CMD_LENGTH];
static char* tokens[MAX_TOKEN_LENGTH];



void parseCommand(){
    //char *delim = " \t\r\n<>|";
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

int getSizeOfCurrCmd(){
    for(int i = 0;i < MAX_CMD_LENGTH;i++){
        if(buffer[i] == '0'){
            return i;
        }
    }
    return MAX_CMD_LENGTH;
}

void printNextLine(){
    memset(buffer,0,MAX_CMD_LENGTH);
    strcpy(buffer,"\nmysh> ");
    write(STDOUT_FILENO,buffer,getSizeOfCurrCmd());
    memset(buffer,0,MAX_CMD_LENGTH);
}

void printCmds(){
    int i = 0;
    while(i < MAX_TOKEN_LENGTH && tokens[i] != NULL){
        printf("%s\n",tokens[i]);
        i++;
    }
}





void InteractiveShell(){
    strcpy(buffer,"Welcome to my Terminal");
    write(STDOUT_FILENO,buffer,getSizeOfCurrCmd());
    printNextLine();
    read(STDIN_FILENO,buffer,MAX_CMD_LENGTH);
    parseCommand();
    printCmds();
    
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