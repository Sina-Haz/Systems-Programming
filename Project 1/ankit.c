#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"
#include "mymalloc.c"

void test1() // malloc and free 120 bytes
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    for (int j = 0; j < 50; j++)
    {
        for (int i = 0; i < 120; i++)
        {
            char *c = malloc(sizeof(char));
            free(c);
        }
    }
    gettimeofday(&endTime, NULL);
    printf("Time for Test 1:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

void freeTask2(char **arr, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        free(arr[i]);
    }
    free(arr);
}

void test2() // malloc 120 in an array and then free 120
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    for (int j = 0; j < 50; j++)
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
    gettimeofday(&endTime, NULL);
    printf("Time for Test 2:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
}

void test3()
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    for (int j = 0; j < 50; j++)
    {
        int current = 0;
        int random = 0;
        char *arr[120];
        for (int i = 0; i < 120; i++)
        {
            arr[i] = NULL;
        }

        while (current < 120)
        {
            random = rand() % 2;
            if (random == 0)
            {
                if (arr[current] != NULL)
                {
                    free(arr[current]);
                    arr[current] = NULL;
                }
            }
            else
            {
                char *temp = malloc(sizeof(char));
                arr[current] = temp;
            }
            current++;
        }

        for (int i = 0; i < 120; i++)
        {
            if (arr[i] != NULL)
            {
                free(arr[i]);
            }
        }
    }
    gettimeofday(&endTime, NULL);
    printf("Time for Test 3:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

void test4() // free backwards
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    for (int j = 0; j < 50; j++)
    {
        char *arr[120];
        for (int i = 0; i < 40; i++)
        {
            double *x = malloc(sizeof(double));
            int *y = malloc(sizeof(char));
            long *z = malloc(sizeof(long));
            free(z);
            free(y);
            free(x);
        }
    }
    gettimeofday(&endTime, NULL);
    printf("Time for Test 4:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

void test5()
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    char *arr[51];
    for (int j = 0; j < 50; j++)
    {
        for (int i = 0; i < 51; i++)
        {
            arr[i] = malloc(sizeof(char));
        }
        for (int i = 0; i <= 25; i++)
        {
            if (i != 0)
            {
                free(arr[25 - i]);
            }
            free(arr[25 + i]);
        }
    }
    gettimeofday(&endTime, NULL);
    printf("Time for Test 5:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

int main(int argc, char *argv[])
{
    test1();
    printMem();
    printf("\n");

    test2();
    printMem();
    printf("\n");

    test3();
    printMem();
    printf("\n");

    test4();
    test5();

    return EXIT_SUCCESS;
}