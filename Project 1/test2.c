#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#include "mymalloc.c"

int main()
{

    double *y = malloc(sizeof(double));

    free(y + 1);
    free(y);
    free(y);

    return EXIT_SUCCESS;
}