#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#include "mymalloc.c"

int main()
{

    int *x = malloc(sizeof(int) * 1018); // this will fill up the heap exactly

    // so now, if we try to malloc something else, we will get an error
    int *y = malloc(sizeof(int));
    // will throw an error

    // BUT, if we free x, we will now be allowed to make  a a new pointer

    free(x);

    int *z = malloc(sizeof(int) * 100);

    printMem();

    return EXIT_SUCCESS;
}