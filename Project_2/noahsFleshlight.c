#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_CMD_LENGTH 100
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100
#define MAX_PATH_LENGTH 80
#define EXTRA 1024

static char buffer[MAX_CMD_LENGTH];
static char *tokens[MAX_TOKEN_LENGTH];
static char extraBuffer[EXTRA];
static char pBuf[MAX_PATH_LENGTH];
char *path;
int wstatus = 0;

// Parses command found in Buffer. Sends tokens to tokens: array of strings
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

void updatePath()
{
    path = getcwd(pBuf, sizeof(pBuf));
}

// checks for null terminator in buffer
int getSizeOfCurrCmd(char *buffer, int len)
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
    memset(tokens, 0, sizeof(tokens));
    char *working_directory = basename(path);
    write(STDOUT_FILENO, working_directory, getSizeOfCurrCmd(working_directory, MAX_PATH_LENGTH));
    if (!wstatus)
    {
        strcpy(buffer, " $: mysh> ");
    }
    else
    {
        strcpy(buffer, " $: !mysh> ");
    }
    write(STDOUT_FILENO, buffer, getSizeOfCurrCmd(buffer, MAX_CMD_LENGTH));
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
        // fixed this error message cause you can's do !wstatus cause wstatus returns 0 or -1, not 0 and 1
        if (wstatus == -1)
        {
            printf("Error: %s does not exist\n", command);
        }
    }
    updatePath();
}

int processCommand()
{
    char *cmd = tokens[0];
    DIR *dp;
    dp = opendir(".");
    struct dirent *dir;
    // struct dirent *dir;

    if (cmd != NULL)
    {
        if (strcmp(cmd, "cd") == 0)
        {
            addToPath(tokens[1], buffer);
        }
        else if (strcmp(cmd, "pwd") == 0)
        {
            wstatus = 0;
            getcwd(buffer, sizeof(buffer));
            printf("the working directory is: %s\n", buffer);
        }
        else if (strcmp(cmd, "echo") == 0)
        {
            printf("%s\n", tokens[1]);
            return 1;
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            while ((dir = readdir(dp)) != NULL)
            {
                printf("%s\n", dir->d_name);
            }
            return 1;
        }
        else if (strcmp(cmd, "mkdir") == 0)
        {
            printf("%s", tokens[1]);
            int result = mkdir(tokens[1], 0700);
            if (result != 0)
            {
                printf("Error creating directory");
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else if (strcmp(cmd, "/") == 0)
        {
            // handles simple executables if you do something like "/ hello" where hello is an exeuctable, not a c file
            pid_t pid;
            int status;
            int tokensLen = 0;
            int i = 0;
            char path[100];

            strcpy(path, "./");
            strcat(path, tokens[1]);

            char **input = malloc(sizeof(char *) * 2);
            input[0] = path;
            input[1] = NULL;

            pid = fork();
            if (pid < 0)
            {
                printf("Error: fork() failed\n");
                return 0;
            }
            else if (pid == 0)
            {
                // Child process
                if (execvp(input[0], input) < 0)
                {
                    printf("Error: execvp() failed\n");
                    return 0;
                }
            }
            else
            {
                // Parent process
                waitpid(pid, &status, 0);
            }

            free(input);
            return 1;
        }
        else if (strcmp(tokens[1], "|") == 0)
        {
            // work in progress
            char *output = malloc(sizeof(char) * 100);
            char *path2 = malloc(sizeof(char) * 100);

            strcpy(path2, "./");
            strcat(path2, tokens[2]);

            FILE *pipe = popen(path2, "r");

            if (pipe == NULL)
            {
                printf("Failed to open pipe to program1\n");
                return 0;
            }

            // Read the output from program1
            if (fgets(output, sizeof(output), pipe) == NULL)
            {
                printf("Failed to read from pipe\n");
                return 0;
            }

            // Print the output from program1
            printf("Program 1 output: %s", output);

            // Close the pipe
            pclose(pipe);

            return 1;
        }
        // else{
        //     int id = fork();
        //     if(id == -1){
        //         perror("Error forking process\n");
        //         exit(1);}
        //     if(id == 0){
        //         wstatus = execvp(cmd,tokens);
        //         perror("Could not execute command");
        //         exit(wstatus);
        //     }
        //     wait(&wstatus);
        //     wstatus = WEXITSTATUS(wstatus);
        // }
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
            perror("Error with Reading command. Try again\n");
            printNextLine();
            continue;
        }
        else
        {
            if (strcmp(buffer, "exit\n") == 0)
            {
                break;
            }
            parseCommand();
            processCommand();
            printNextLine();
        }
    }
}

void InteractiveShell()
{
    path = malloc(sizeof(char) * MAX_CMD_LENGTH);
    updatePath();
    strcpy(buffer, "Welcome to my Terminal\n");
    write(STDOUT_FILENO, buffer, getSizeOfCurrCmd(buffer, MAX_CMD_LENGTH));
    printNextLine();
    CommandLoop(STDIN_FILENO);
}

void BatchShell(int fd)
{
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        int fd = open(argv[1], O_RDONLY);

        if (fd == -1)
        {
            printf("Error with opening file argument");
            return EXIT_FAILURE;
        }

        BatchShell(fd);
    }
    InteractiveShell();

    return EXIT_SUCCESS;
}