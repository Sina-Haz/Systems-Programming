#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    clock_t start, end;
    double time_taken;

    start = clock();
    system("top -bn1 | grep \"^ *PID\\|%s\" | grep -v grep | ls | cd ..");
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to run command using other terminals: %f seconds\n", time_taken);


    start = clock();
    system("./mysh benchmark.txt");
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to run commmand using my terminal: %f seconds\n", time_taken);



}