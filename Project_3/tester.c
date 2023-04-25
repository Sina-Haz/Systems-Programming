#include<stdio.h>
#include<stdlib.h>
#include "message.h"
#include "message.c"

int main(){
    strcpy(msg_buf,"DRAW|3|S|");
    int read_bytes = strlen(msg_buf);
    printf("%d\n",identify_msg(read_bytes));
}