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

char *wildChar(int func, char *str);
int wildCard(char **globs, char *start, char *end);


// Parses command found in Buffer. Sends tokens to tokens: array of strings
void saveStdFd(){
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
}

void processCommand(char* cmd, int token_ind, int shouldHandleBar);
void addToPath(char *command, char *buf);
char** getArgsFromTokens(int startInd);

void parseCommand()
{
    int num_tokens = 0;
    int i = 0;
    int current_token_length = 0;
    int in_token = 0;

    char **tempArr = malloc(sizeof(char *) * MAX_TOKENS);
    for (int i = 0; i < MAX_TOKENS; i++)
    {
        tempArr[i] = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
    }

    while (i < MAX_CMD_LENGTH && buffer[i] != '\0')
    {
        if (buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|' || buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\t')
        {
            if (in_token)
            {
                tempArr[num_tokens][current_token_length] = '\0';
                num_tokens++;
                current_token_length = 0;
                in_token = 0;
            }
            if (buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '|')
            {
                tempArr[num_tokens] = malloc(4 * sizeof(char));
                tempArr[num_tokens][0] = buffer[i];
                tempArr[num_tokens][1] = '\0';
                num_tokens++;
            }
        }
        else
        {
            if (!in_token)
            {
                tempArr[num_tokens] = malloc(MAX_TOKEN_LENGTH * sizeof(char));
                in_token = 1;
            }
            tempArr[num_tokens][current_token_length] = buffer[i];

            current_token_length++;
        }
        i++;
    }

    if (in_token)
    {
        tempArr[num_tokens][current_token_length] = '\0';
        num_tokens++;
    }

    // WILDCARD
    // Plan: check if tokens[index] contains an asterik.  If it does, we can use
    // my wildChar function to get the start and the end, and then my other wildcard function
    // to make a 2D array of everything matching the wildcard

    int tokInd = 0;
    for (int k = 0; k < num_tokens; k++)
    {
        char *start = malloc(sizeof(char) * 50);
        char *end = malloc(sizeof(char) * 50);
        start = wildChar(0, tempArr[k]);
        end = wildChar(1, tempArr[k]);
        if (start == NULL && end == NULL)
        {
            // tokens[tokInd] = tempArr[k];
            tokens[tokInd] = strdup(tempArr[k]);
            tokInd++;
        }
        else
        {
            char **globs = malloc(sizeof(char *) * MAX_TOKENS);
            for (int i = 0; i < MAX_TOKENS; i++)
            {
                globs[i] = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
            }
            int lenWild = wildCard(globs, start, end);
            // globs = realloc(globs, lenWild * sizeof(char *));
            for (int i = 0; i < lenWild; i++)
            {
                tokens[tokInd] = strdup(globs[i]);
                tokInd++;
            }
            for(int i = 0; i < MAX_TOKENS; i++) {
                free(globs[i]);
            }
            free(globs);
        }
        free(start);
        free(end);
    }

    for (int i = 0; i < 100; i++)
    {
        free(tempArr[i]);
    }
    free(tempArr);

}


// this is the first of 2 functions used in dealing with wildcards
// this function takes in a string and a number 0 or 1.  We input 0 when we want to return everything before the
// asterik and 1 when we want everything after
// the string we insert is teh string containing the asterik taht we want to break in two
char *wildChar(int func, char *str)
{
    // if func is 0, we want before asterik, if it's 1, we want after asterik;
    int astIndex = -1;
    char *output;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == '*')
        {
            astIndex = i;
        }
    }
    if ((astIndex == -1) || (astIndex == 0 && func == 0))
    {
        return NULL;
    }
    // substr = strndup(str + 7, 5);  // Get a substring starting at position 7 with length 5
    if (func == 0)
    {
        output = strndup(str, astIndex); // Get a substring starting at position 7 with length 5
    }
    else
    {
        output = strndup(str + astIndex + 1, strlen(str) - astIndex);
    }

    return output;
}

// this is the second of two wildcard functions.  This one does the bulk of the work.  It takes in
// a char* representing the start of the file name and a char* representing the end of the file name, and returns
// a 2d array of everything in the directory matching those attributes.  If start is NULL, or end is NULL< it still
// serves the same purpose
int wildCard(char **globs, char *start, char *end)
{
    DIR *d;
    struct dirent *dir;

    int file_count = 0;
    //int length = 0;

    d = opendir(".");

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (start == NULL)
            {
                if (strstr(dir->d_name, end))
                {
                    strncpy(globs[file_count], dir->d_name, 100);
                    file_count++;
                    if (file_count >= 100)
                    {
                        printf("Maximum number of files exceeded.\n");
                        break;
                    }
                }
            }
            else
            {
                int n = strlen(start);
                if ((strncmp(start, dir->d_name, n) == 0) && (strstr(dir->d_name, end)))
                {
                    strncpy(globs[file_count], dir->d_name, 100);
                    file_count++;
                }
            }
        }
        closedir(d);
    }
    return file_count;
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
        int fd = open(tokens[symbolInd+1], O_WRONLY | O_CREAT, 0640);
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

int HandleBar(int BarInd, int CurrInd){
    if(BarInd != 0 && BarInd != -1){
        int fds[2];
        if(pipe(fds) == -1){
            perror("pipe");
            return -1;
        }
        int pid = fork();
        if(pid == -1){
            perror("fork");
            return -1;
        }
        
        if(pid == 0){
            //child process should run the 2nd sub-commmand
            close(fds[1]);
            if(dup2(fds[0],STDIN_FILENO) == -1){perror("dup error");}
            processCommand(tokens[BarInd+1],BarInd+1,1);
            close(fds[0]);
            exit(wstatus);
        }
        else{
            //parent process runs the 1st sub-command
            close(fds[0]);
            if(dup2(fds[1],STDOUT_FILENO) == -1){perror("dup error");
            return -1;}
            processCommand(tokens[CurrInd],CurrInd,0);
            wait(&wstatus);
            wstatus = WEXITSTATUS(wstatus);
            close(fds[1]);
            dup2(saved_stdin,STDIN_FILENO);
            dup2(saved_stdout,STDOUT_FILENO);
            return 1;
        }
    }
    else{
        return -1;
    }

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

void processCommand(char* cmd, int token_ind, int shouldHandleBar)
{
    int symbol_handling = 0;
    int symbolInd = searchForSymbol(token_ind);
    int BarInd = searchForBar(token_ind);
    if (BarInd >= 0 && shouldHandleBar == 1){
        if(HandleBar(BarInd,token_ind) == 1){
            return;
        }
    }

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
    }else{
        perror("error processing command");
        wstatus = 1;
    }

    if(symbolInd != -1 && symbol_handling != -1){
        if(strcmp(tokens[symbolInd],">") == 0){
            dup2(saved_stdout,STDOUT_FILENO);
        }
        else if(strcmp(tokens[symbolInd],"<") == 0){
            dup2(saved_stdin,STDIN_FILENO);
        }
    }

    // if(BarInd != -1 && BarInd != 0){
    //     processCommand(tokens[BarInd+1],BarInd+1,1);
    // }

}



void CommandLoop(int fd)
{
    //ssize_t bytes_read;
    char* ptr;
    int ExtraInput = 0;
    if(fd == STDIN_FILENO){
        ExtraInput = CheckForExtraInput(fd);
    }
    FILE* fp = fdopen(fd,"r");
    while (1)
    {
        ptr = fgets(buffer,MAX_CMD_LENGTH,fp);
        if (ExtraInput == 1)
        {
            perror("Error with Reading command\n");
            printNextLine();
            continue;;
        }
        else
        {
            if (strcmp(buffer, "exit\n") == 0 || ptr == NULL)
            {
                break;
            }
            parseCommand();
            //printCmds();
            processCommand(tokens[0],0,1);
            printNextLine();
        }
    }
    fclose(fp);
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
    path = malloc(sizeof(char)*MAX_CMD_LENGTH);
    updatePath();
    printNextLine();
    CommandLoop(fd);
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