#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#include "mymalloc.c"

int main()
{

    int **x = malloc(sizeof(int *) * 10); // this makes space for an array of 10 integers
    // we will fill it and then it up individually, then finally freeing the array itself.
    for (int i = 0; i < 10; i++)
    {
        int *y = malloc(sizeof(int));
        x[i] = y;
    }
    for (int i = 9; i >= 0; i--)
    {
        free(x[i]);
    }
    free(x);

    // freeing backwards
    printMem();

    return EXIT_SUCCESS;
}