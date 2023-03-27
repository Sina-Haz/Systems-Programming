#include <stdio.h>
#include<stdlib.h>

int main() {
    char buffer[100];
    printf("Please enter some text: ");
    fgets(buffer, 100, stdin);
    printf("You entered: %s", buffer);
    return 0;
}