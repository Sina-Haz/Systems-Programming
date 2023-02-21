#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mymalloc.h"
#include "mymalloc.c"

// this method for the first part of memgrind, where me make a 1 byte chunk and free it 120 times
void loopyStuff()
{
    for (int i = 0; i < 120; i++)
    {
        char *c = malloc(sizeof(char));
        free(c);
    }
}

void loops2theSequel(char **storage)
{
    for (int i = 0; i < 120; i++)
    {
        char *c = malloc(sizeof(char));
        storage[i] = c;
    }
}

void loops3TheFreeing(char **storage)
{
    for (int i = 0; i < 120; i++)
    {
        free(storage[i]);
    }
}

int main()
{
    long arrays[50];
    // first part:

    long avg1 = 0;
    for (int k = 0; k < 50; k++)
    {
        clock_t start = clock();
        loopyStuff();
        clock_t end = clock();
        arrays[k] = (end - start) * (0.000001) / (double)CLOCKS_PER_SEC; // in microseconds
    }
    long total = 0;
    for (int i = 0; i < 50; i++)
    {
        total += arrays[i];
    }
    total /= 50;
    printf("Time average measured for task 1 is %lli microseconds.\n", total);

    // part 2 starts below:
    //      2. Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to
    //  deallocate the chunks.

    long avg2 = 0;
    long arrays2[50];
    for (int k = 0; k < 50; k++)
    {
        char **storage = malloc(sizeof(char *) * 120); // will be used to store the pointers we're freeing
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

    return 0;
}