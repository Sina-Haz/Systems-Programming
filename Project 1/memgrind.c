#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"
#include "mymalloc.c"

#define TASK_COUNT 4
#define RUN_COUNT 50

// this method for the first part of memgrind, where me make a 1 byte chunk and free it 120 times
void Task1()
{
    for (int i = 0; i < 120; i++)
    {
        char *c = malloc(sizeof(char));
        free(c);
    }
}

void freeTask2(char **arr, int len);

void Task2()
{
    int len = 120;
    char **arr = malloc(sizeof(char *) * len); // make a 120 length array of char pointers
    for (int i = 0; i < len; i++)
    {
        char *byte = malloc(sizeof(char));
        arr[i] = byte;
    }
    freeTask2(arr, len);
}

void freeTask2(char **arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        free(arr[i]);
    }
    free(arr);
}


int findIndOfOne(int* arr, int len){
    for(int i = 0; i < len;i++){
        if(arr[i] == 1){
            return i;
        }
    }
    return -1;
}


void printRepArr(int *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d,", arr[i]);
        if (i % 12 == 0 && i != 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

int* initializedArr(int len){
    int* arr = malloc(sizeof(int)*len);
    for(int i = 0;i < len;i++){
        arr[i] = 0;
    }
    return arr;
}

void Task3()
{
    int mallocCalls = 0;
    int freeCalls = 0;
    int len = 30;
    char* arr[len]; //malloc array of char pointers of size 120
    int* repArr = initializedArr(len);

    while(mallocCalls < len){
        int random = rand() % 2;
        if(random == 0){
            char* byte = malloc(sizeof(char));
            arr[mallocCalls] = byte;
            repArr[mallocCalls] = 1;
            mallocCalls++;
        }

        else if(random == 1){
            int ind = findIndOfOne(repArr,len);
            if(ind != -1){
                free(arr[ind]);
                repArr[ind] = 0;
            }
            freeCalls++;
        }
        printRepArr(repArr,len);
    }

    printRepArr(repArr,len);
}




int main()
{

    /*
    long arrays[RUN_COUNT];
    // first part: allocate and immediately free 120 1-byte chunks. Repeat this fifty times


    long avg1 = 0;
    struct timeval start,end;
    for (int k = 0; k < RUN_COUNT; k++)
    {
        gettimeofday(&start,NULL);
        Task1();
        gettimeofday(&end,NULL);
        arrays[k] = (double) (end.tv_sec - start.tv_sec) * 1000000
                          + (double) (end.tv_usec - start.tv_usec); //in microseconds
    }

    for (int i = 0; i < RUN_COUNT; i++)
    {
        avg1 += arrays[i];
    }
    avg1 /= RUN_COUNT;
    printf("Time average measured for task 1 is %li microseconds.\n", avg1);
    */

    Task2();

    Task3();

    /*

    // part 2 starts below:
    //      2. Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to
    //  deallocate the chunks.

    long avg2 = 0;
    long arrays2[50];
    for (int k = 0; k < 50; k++)
    {
        char storage[120][1]; // will be used to store the pointers we're freeing
        clock_t start2 = clock();
        loops2theSequel(storage);
        printf("breakpoint 1");
        loops3TheFreeing(storage);
        clock_t end2 = clock();
        arrays2[k] = (end2 - start2) * (0.000001) / (double)CLOCKS_PER_SEC; // in microseconds
    }
    long total2 = 0;
    for (int i = 0; i < 50; i++)
    {
        total2 += arrays2[i];
    }
    total2 /= 50;
    printf("Time average measured for task 2 is %lli seconds.\n", total2);
    */

    return 0;
}