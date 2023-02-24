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


void freeCharPtrArr(char **arr, int len)
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
        freeCharPtrArr(arr, len);
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
        int mallocCount = 0;
        char **array = malloc(sizeof(char *) * 120);
        int checker[120];
        for (int i = 0; i < 120; i++)
        {
            checker[i] = 0;
        }

        while (mallocCount < 120)
        {

            int randomNumber = rand() % (3 - 1) + 1;

            if (randomNumber == 1)
            {
                char *c = malloc(sizeof(char));
                array[mallocCount] = c;
                checker[mallocCount] = 1;
                mallocCount++;
            }
            else
            {
                int index = 119;
                int deleted = 0;
                while (deleted == 0 && index >= 0)
                {
                    if (checker[index] == 1)
                    {
                        free(array[index]);
                        checker[index] = 0;
                        deleted = 1;
                    }
                    else
                    {
                        index--;
                    }
                }
            }
        }
        int k = 119;
        while (k >= 0)
        {
            if (checker[k] == 1)
            {
                free(array[k]);
                checker[k] = 0;
                k--;
            }
            else
            {
                k--;
            }
        }
        free(array);
    }
    gettimeofday(&endTime, NULL);
    printf("Time for Test 3:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

void test4() // three differnet data types
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    for (int j = 0; j < 50; j++)
    {
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

void test5Loop();

void test5()
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    test5Loop(); // dummy placeholder method
    gettimeofday(&endTime, NULL);
    printf("Time for Test 5:  %ld microseconds\n", (((endTime.tv_sec - startTime.tv_sec) * 100000) + (endTime.tv_usec - startTime.tv_usec)) / 50);
    return;
}

void test5Loop()
{
    for (int j = 0; j < 1000; j++)
    {
        char *dummy = malloc(sizeof(char));
        free(dummy);
    }
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
    printMem();
    printf("\n");

    test5();
    printMem();
    printf("\n");

    return EXIT_SUCCESS;
}