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

// void Task2(){
//     int len = 120;
//     char** arr = malloc(sizeof(char*) * len); //make a 120 length array of char pointers
//     for(int i = 0;i < len;i++){
//         arr[i] = malloc(sizeof(char));
//     }
//     for(int i = 0; i < len;i++){
//         free(arr[i]);
//     }
//     free(arr);
// }

void Task2() {
    int len = 120;
    char *arr[len];
    for(int i = 0; i < len; i++) {
        char *c = malloc(sizeof(char));
        arr[i] = c;
    }
    for(int i = 0; i < len; i++) {
        free(arr[i]);
    }
}


// so basically task 3 randomly chooses between mallocating or freeing 120 times
// so I create an int thats randomly assigned the value of 1 or 2, where one means we malloc and two means we free
// we repeat this until we've malloced 120 times, hence the malloc counter
// the problem is that this gives a bad free error
void task3() {
    int mallocCount = 0;
    int len = 120;
    char *arr[len];
    for(int i = 0; i < 120; i++) {
        arr[i] = NULL;
    }

    // so far, this works in the sense that it randomly chooses until we've hit malloc 120 times, the problem arises in the free portion
    // how can we free more times than we malloc
    while(mallocCount < 120) {
        int randomnumber;
        randomnumber = rand()%(3-1) + 1; //generates random num. either 1 or 2, so a 1 will be malloc and 2 will be free
        if(randomnumber == 1) {
            char *c = malloc(sizeof(char));
            arr[mallocCount] = c;
            mallocCount++;
        } else {
            int deleted = 0;
            int index = 0;
            while(deleted != 1 && index < 120) {
                if(arr[index] != NULL) {
                    free(arr[index]);
                    arr[index] = NULL;
                    deleted = 1;
                } else {
                    index++;
                }
            }
        }
    }
    //now we clean up all remaining elements, if any
    int index = 0;
    while(index < 120) {
        if(arr[index] != NULL) {
            free(arr[index]);
            arr[index] = NULL;
            index++;
        } else {
            index++;
        }
    }

}

/*
void loops2theSequel(char storage[120][1])
{
    for (int i = 0; i < 120; i++)
    {
        char *c = malloc(sizeof(char));
        storage[i][0] = (int*) c;
    }
}

void loops3TheFreeing(char storage[120][1])
{
    for (int i = 0; i < 120; i++)
    {
        free(storage[i][0]);
    }
}
*/


int main()
{
    
    
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

    long avg2 = 0;
    long arrays2[RUN_COUNT];

    for (int k = 0; k < RUN_COUNT; k++)
    {
        gettimeofday(&start,NULL);
        Task2();
        gettimeofday(&end,NULL);
        arrays2[k] = (double) (end.tv_sec - start.tv_sec) * 1000000
                          + (double) (end.tv_usec - start.tv_usec); //in microseconds
    }

    for (int i = 0; i < RUN_COUNT; i++)
    {
        avg2 += arrays2[i];
    }
    avg2 /= RUN_COUNT;
    printf("Time average measured for task 2 is %li microseconds.\n", avg2);

    long avg3 = 0;
    long arrays3[RUN_COUNT];

    for (int k = 0; k < RUN_COUNT; k++)
    {
        gettimeofday(&start,NULL);
        task3();
        gettimeofday(&end,NULL);
        arrays3[k] = (double) (end.tv_sec - start.tv_sec) * 1000000
                          + (double) (end.tv_usec - start.tv_usec); //in microseconds
    }

    for (int i = 0; i < RUN_COUNT; i++)
    {
        avg3 += arrays3[i];
    }
    avg3 /= RUN_COUNT;
    printf("Time average measured for task 3 is %li microseconds.\n", avg3);



    return 0;
}