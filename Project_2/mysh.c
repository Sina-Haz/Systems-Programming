#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include<libgen.h>

#define MAX_CMD_LENGTH 100
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100
#define MAX_PATH_LENGTH 80
#define EXTRA 1024

static char buffer[MAX_CMD_LENGTH];
static char *tokens[MAX_TOKEN_LENGTH];
static char extraBuffer[EXTRA];
static char pBuf[MAX_PATH_LENGTH];
char* path;
int wstatus = 0;
int saved_stdin;
int saved_stdout;


// Parses command found in Buffer. Sends tokens to tokens: array of strings
void saveStdFd(){
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
}

void parseCommand()
{
    int num_tokens = 0;
    int i = 0;
    int current_token_length = 0;
    int in_token = 0;

    while (i < MAX_CMD_LENGTH && buffer[i] != '\0')
    {
        if (buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|' || buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\t')
        {
            if (in_token)
            {
                tokens[num_tokens][current_token_length] = '\0';
                num_tokens++;
                current_token_length = 0;
                in_token = 0;
            }
            if (buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|')
            {
                tokens[num_tokens] = malloc(4 * sizeof(char));
                tokens[num_tokens][0] = buffer[i];
                tokens[num_tokens][1] = '\0';
                num_tokens++;
            }
        }
        else
        {
            if (!in_token)
            {
                tokens[num_tokens] = malloc(MAX_TOKEN_LENGTH * sizeof(char));
                in_token = 1;
            }
            tokens[num_tokens][current_token_length] = buffer[i];
            current_token_length++;
        }
        i++;
    }

    if (in_token)
    {
        tokens[num_tokens][current_token_length] = '\0';
        num_tokens++;
    }
}

void updatePath(){
    path = getcwd(pBuf,sizeof(pBuf));
}

// checks for null terminator in buffer
int getSizeOfCurrCmd(char* buffer,int len)
{
    for (int i = 0; i < len; i++)
    {
        if (buffer[i] == '\0')
        {
            return i;
        }
    }
    return len;
}

// Prints next line in interactive mode. Should recognize if previous command caused an error and add !
void printNextLine()
{
    memset(buffer, 0, MAX_CMD_LENGTH);
    memset(tokens,0,sizeof(tokens));
    char* working_directory = basename(path);
    write(STDOUT_FILENO,working_directory,getSizeOfCurrCmd(working_directory,MAX_PATH_LENGTH));
    if(wstatus == 0){
        strcpy(buffer, " $: mysh> ");
    }
    else{
        strcpy(buffer, " $: !mysh> ");
    }
    write(STDOUT_FILENO, buffer, getSizeOfCurrCmd(buffer,MAX_CMD_LENGTH));
    memset(buffer, 0, MAX_CMD_LENGTH);
}

// Helper method to print tokens after parsing command
void printCmds()
{
    int i = 0;
    while (i < MAX_TOKEN_LENGTH && tokens[i] != NULL)
    {
        printf("%s\n", tokens[i]);
        i++;
    }
}

//returns index within tokens where there is a '|'
int searchForBar(int startInd){
    char* curr = tokens[startInd];
    while(curr != NULL){
        if(strcmp(curr,"|") == 0){return startInd;}
        startInd++;
        curr = tokens[startInd];
    }
    return -1;
}

//searches tokens for < or > symbols
int searchForSymbol(int startInd){
    char* curr = tokens[startInd];

    //searches for symbol until hits a NULL or |
    while(curr != NULL && strcmp(curr,"|") != 0){
        if(strcmp(curr,">") == 0 || strcmp(curr,"<") == 0){return startInd;}
        startInd++;
        curr = tokens[startInd];
    }
    return -1;
}

int HandleSymbol(int symbolInd){
    if(symbolInd == 0 || tokens[symbolInd + 1] == NULL || strcmp(tokens[symbolInd + 1],">") == 0 ||
     strcmp(tokens[symbolInd+1],"<") == 0 || strcmp(tokens[symbolInd+1],"|") == 0){
        return -1;
     }
     else if(strcmp(tokens[symbolInd],">") == 0){
        int fd = open(tokens[symbolInd+1], O_WRONLY | O_CREAT, 0777);
        int fd2 = dup2(fd,STDOUT_FILENO);
        close(fd);
        return fd2;
    }
    else{
        int fd = open(tokens[symbolInd+1], O_RDONLY);
        int fd2 = dup2(fd,STDIN_FILENO);
        close(fd);
        return fd2;
    }
    return -1;
}

// Checks if the input command is longer than the buffer by setting fd to non-blocking mode and
// trying to read any extra input.
int CheckForExtraInput(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    ssize_t extra_bytes_read = read(fd, extraBuffer, EXTRA);
    fcntl(fd, F_SETFL, flags);
    if (extra_bytes_read != -1)
    {
        return 1;
    }
    memset(extraBuffer, 0, EXTRA);
    return 0;
}

char** getArgsFromTokens(int startInd){
    int num_tokens = 0;
    while(tokens[startInd+num_tokens] != NULL && strcmp(tokens[startInd+num_tokens],"|") != 0 && 
    strcmp(tokens[startInd+num_tokens],">") != 0 && strcmp(tokens[startInd+num_tokens],"<") != 0){
        num_tokens++;
    }

    char** args = malloc(sizeof(char*)*(num_tokens+1));
    int token_count = 0;
    for(int i = 0;i < num_tokens;i++){
        args[i] = strdup(tokens[startInd+token_count]);
        token_count++;
    }
    args[num_tokens] = NULL;
    return args;
}


// this will be used in conjunction with the cd call
void addToPath(char *command, char *buf)
{
    if (strcmp(command, "..") == 0)
    {
        wstatus = chdir("..");
    }
    else
    {
        getcwd(buf, sizeof(buf));
        wstatus = chdir(command);
        if(wstatus == -1){
            printf("Error: %s does not exist\n",command);
        }
    }
    updatePath();
}

void processCommand(char* cmd, int token_ind)
{
    // DIR *dp;
    // dp = opendir(".");
    //struct dirent *dir;
    int symbol_handling = 0;
    int symbolInd = searchForSymbol(token_ind);
    int BarInd = searchForBar(token_ind);

    if(symbolInd != -1){
        symbol_handling = HandleSymbol(symbolInd);
        if(symbol_handling == -1){wstatus = 1;}
    }

    if(cmd != NULL && symbol_handling != -1){
        if(strcmp(cmd, "cd") == 0)
        {
            addToPath(tokens[token_ind+1], buffer);
        }
        else if (strcmp(cmd, "pwd") == 0){
            wstatus = 0;
            getcwd(buffer, sizeof(buffer));
            printf("the working directory is: %s\n", buffer);
        }
        else{
            int id = fork();
            char** args = getArgsFromTokens(token_ind);
            if(id == -1){
                perror("Error forking process");
                exit(1);}
            if(id == 0){
                wstatus = execvp(cmd,args);
                perror("Could not execute command");
                exit(wstatus);
            }
            wait(&wstatus);
            wstatus = WEXITSTATUS(wstatus);
        }
    }

    if(symbolInd != -1 && symbol_handling != -1){
        if(strcmp(tokens[symbolInd],">") == 0){
            dup2(saved_stdout,STDOUT_FILENO);
        }
        else if(strcmp(tokens[symbolInd],"<") == 0){
            dup2(saved_stdin,STDIN_FILENO);
        }
    }

    if(BarInd != -1 && BarInd != 0){
        processCommand(tokens[BarInd+1],BarInd+1);
    }

}



void CommandLoop(int fd)
{
    ssize_t bytes_read;
    while (1)
    {
        bytes_read = read(fd, buffer, MAX_CMD_LENGTH);
        if (bytes_read == -1 || CheckForExtraInput(fd) == 1)
        {
            perror("Error with Reading command\n");
            exit(1);
        }
        else
        {
            if (strcmp(buffer, "exit\n") == 0)
            {
                break;
            }
            parseCommand();
            processCommand(tokens[0],0);
            printNextLine();
        }
    }
}

void InteractiveShell()
{
    path = malloc(sizeof(char)*MAX_CMD_LENGTH);
    updatePath();
    strcpy(buffer, "Welcome to my Terminal\n");
    write(STDOUT_FILENO, buffer, getSizeOfCurrCmd(buffer,MAX_CMD_LENGTH));
    printNextLine();
    CommandLoop(STDIN_FILENO);
}

void BatchShell(int fd)
{
    printf("Mashallah we are in the bash shell!\n");
}

int main(int argc, char *argv[])
{
    saveStdFd();
    if (argc > 1)
    {
        int fd = open(argv[1], O_RDONLY);

        if (fd == -1)
        {
            printf("Error with opening file argument\n");
            return EXIT_FAILURE;
        }

        BatchShell(fd);
    }else{
        InteractiveShell();
    }



    return EXIT_SUCCESS;
}