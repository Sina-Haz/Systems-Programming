#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#include "mymalloc.c"

int main()
{
    int *arr = malloc(5 * sizeof(int));
    arr[3] = 28;

    char *string = malloc(45 * sizeof(char));

    long *arr2 = malloc(38 * sizeof(long));
    arr2[20] = 69;

    string = memcpy(string, "Hello my name is Sina Hazeghi", 30);

    FreeAll();

    free(string);

    free(arr);

    free(arr2);

    free(string);

    int num = 0;

    free(&num);

    free(arr + 2);

    free(arr);

    free(arr);
}