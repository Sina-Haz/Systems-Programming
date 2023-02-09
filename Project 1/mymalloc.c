#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"


#define HEAP_SIZE 4096
static char heap[HEAP_SIZE];
char *ind = heap;



void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);

int main()
{
    printf("%d\n", ind);
    ind += 8;

    memset(heap, 0, 4096); // test line
    int *x = malloc(sizeof(int));
    double *y = malloc(sizeof(double));
    int *k = malloc(sizeof(int));

    for (int i = 0; i < 50; i++)
    {
        printf("%d, ", heap[i]);
    }

    // so far, all the addresses space out appropriately, but I don't know how to access the char content, given a pointer

    return EXIT_SUCCESS;
}

// to use memset, we need a pointer to a spot in memory, what we want to replace, then how many spots to replace
void *mymalloc(size_t size, char *file, int line)
{
    char *header = ind;
    // size bit:
    memset(ind - 8, size, 4);
    // valid bit:
    memset(ind - 4, 1, 4);

    memset(ind, 'x', size);
    ind += (size + 8);

    return header;

    // we need to return a pointer to the head of this data
}
void myfree(void *ptr, char *file, int line)
{
    int size = 0;
    memset(ptr - 8, 0, 8);

    // so this clears the known metadata, but it does not clear everything after
}

// QUESTIONS:
// 1. ask about the "size", "file", and "line" inputs, make sure they're really necessary since usually
// malloc only takes in one argument
// 2. How can we keep track of where we are in the char array

