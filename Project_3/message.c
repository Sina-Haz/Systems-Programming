#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#define MSG_LEN 264
#define CMD_LEN 4
#define MAX_TOTAL_FIELDS 5
#define MALFORMED_MSG -5
#define PROPER_MSG 1
#define MISSING_INFO 2
#define OVERFLOW_MSG 3

char msg_buf[MSG_LEN];
char* msg_fields[MAX_TOTAL_FIELDS];
int curr_fields = 0;

//to be used for the first message if there's overflow
char* first_msg;

//message format: CMD4|#|...|

int getNumFields(char* cmd){
    int num = -1;
    if(strcmp(cmd,"WAIT") == 0){
        num = 2;
    }else if(strcmp(cmd,"NAME") == 0){
        num = 3;
    }else if(strcmp(cmd,"MOVE") == 0){
        num = 4;
    }else if(strcmp(cmd,"MOVD") == 0){
        num = 5;
    }else if(strcmp(cmd,"INVL") == 0){
        num = 3;
    }else if(strcmp(cmd,"DRAW") == 0){
        num = 3;
    }else if(strcmp(cmd,"OVER") == 0){
        num = 4;
    }

    return num;
}

//gets all fields into their own tokens
int get_msg_tokens(){
    if(msg_buf[0] == '\0'){
        perror("nothing in message buffer.");
        return -1;
    }
    char* msg_copy = strdup(msg_buf);
    char* token = strtok(msg_copy,"|");
    int i = 0;
    do{
        if(i == MAX_TOTAL_FIELDS){
            free(msg_copy);
            curr_fields = i;
            return -2;
        }
        msg_fields[i] = strdup(token);
        token = strtok(NULL,"|");
        i++;

    }while(token != NULL);

    free(msg_copy);
    curr_fields = i;
    return curr_fields;
}

//this is a helper method that extracts the rest of the substring after the command and the number
char* extract_substr(){
    int i = 0;
    char* ptr = msg_buf;
    while(*ptr != '\0' && i < 2){
        if(*ptr == '|'){i++;}
        ptr++;
    }
    if(i < 2){return NULL;}
    return ptr;
}


//figure out if whatever in msg_buf is a complete message
int identify_msg(int read_bytes){
    //assume we call read and we know how many bytes it got.

    if(read_bytes < 0){
        perror("read error!");
        return -1;
    }else if(read_bytes == 0){
        printf("Connection has been closed!");
        return 0;
    }
    int num = get_msg_tokens();
    if(num == -1){
        perror("Message was NULL!");
        return -2;
    }else if(num == -2){
        printf("too many fields in the message.\n");
        return OVERFLOW_MSG;
    }
    if(curr_fields < 2){
        printf("Not enough fields for proper message.\n");
        return MISSING_INFO;
    }

    char* cmd;
    int num_bytes, rt_num_fields;

    cmd = msg_fields[0];
    num_bytes = atoi(msg_fields[1]);

    if(num_bytes > MSG_LEN){
        perror("Message byte length exceeded");
        return -3;
    }
    char* substr = extract_substr();

    if(strlen(substr) < num_bytes){
        return MISSING_INFO;
    }
    else if(strlen(substr) > num_bytes){
        printf("wrong length\n");
        return MALFORMED_MSG;
    }

    rt_num_fields = getNumFields(cmd);
    if(rt_num_fields > curr_fields){
        return MISSING_INFO;
    }else if(rt_num_fields < curr_fields){
        return OVERFLOW_MSG;
    }
    return PROPER_MSG;
}

int handle_overflow(){
    int num_bytes = atoi(msg_fields[1]);
    char* substr = extract_substr();
    if(substr[num_bytes-1] != '|'){
        return MALFORMED_MSG;
    }
    //haven't checked for right amt of fields in first message but oh well im too lazy to do it rn

    //save the first message
    int len_first_msg = &substr[num_bytes] - msg_buf;
    first_msg = malloc(sizeof(char)*(len_first_msg+2));
    memcpy(first_msg,msg_buf,len_first_msg);
    first_msg[len_first_msg] = '\0';

    printf("first message is: %s\n",first_msg);

    char* new_buf = malloc(sizeof(char)*MSG_LEN);
    memcpy(new_buf,msg_buf+len_first_msg,MSG_LEN - len_first_msg);
    bzero(msg_buf,MSG_LEN);
    memcpy(msg_buf,new_buf,MSG_LEN);
    free(new_buf);
    printf("second message is: %s\n",msg_buf);

    return PROPER_MSG;
}





//this file will have functions used to handle recieving and sending messages from server
int main(int argc, char* argv[]){

    strcpy(msg_buf,"WAIT|0|");
    int read_bytes = strlen(msg_buf);
    printf("%d\n",identify_msg(read_bytes));

    bzero(msg_buf,MSG_LEN);
    strcpy(msg_buf,"NAME|10|Joe Smith|");
    printf("%d\n",identify_msg(read_bytes));

    bzero(msg_buf,MSG_LEN);
    strcpy(msg_buf,"NAME|10|Joe Smith|Power Ranger|");
    printf("%d\n",identify_msg(read_bytes));
    handle_overflow();

    return EXIT_SUCCESS;
}