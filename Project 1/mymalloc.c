#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

#define HEAP_SIZE 4096
static char heap[HEAP_SIZE];
char *headofmem = &heap[0];
int first = 0;
// will always be the beginning of memory

void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);

typedef struct chunk
{
    struct chunk *next;
    struct chunk *prev;
    int size;              // size of chunk
    unsigned char *buffer; // head of memory for the chunk

    // now, the question is, how do we store this in the array?
} chunk;

chunk *linkedList;
void printlist(chunk *ptr);

void initMyMalloc()
{
    linkedList = (struct chunk *)heap; // cast heap to a struct pointer
    linkedList->next = NULL;
    linkedList->prev = NULL;
    linkedList->size = sizeof(heap) - sizeof(struct chunk); // this creates an empty chunk of size 4080 (4096-16(size of-
    // a chunk struct))
    linkedList->buffer = (unsigned char *)(heap + sizeof(struct chunk));

    // this initalizes a global chunk node that we will need in order to be able to insert into the heap
    //  everything in here is correct because I had a TA help me set it up, so we need to use this to add
    //  to the heap by creating new node "chunks"
}

void printlist(chunk *ptr)
{
    int i = 0;
    while (ptr != NULL)
    {
        printf("Chunk: %d\n", i);
        printf("Size: %d\n", ptr->size);
        printf("Current Buffer: %d\n", ptr->buffer);
        ptr = ptr->next;
        i++;
    }
}

// here's why I got my inspiration: https://sites.cs.ucsb.edu/~rich/class/cs170/labs/lab1.malloc/what_i_did.html

int main()
{
    initMyMalloc(); // this gives our global variable some basic attributes that will be overwritten on the first call to malloc
    int *x = malloc(sizeof(int));
    int *y = malloc(sizeof(int));

    printlist(linkedList);

    // thinks linkedlist->next is NULL

    return EXIT_SUCCESS;
}

void *mymalloc(size_t size, char *file, int line)
{
    // we have linked list as the head
    // the goal here is to insert a new chunk object with only a global pointer and a size
    // if you fix this I'll kiss you on the mouth
    chunk *new_chunk = (struct chunk *)heap;

    if (first == 0)
    {
        first++;
        new_chunk->size = (size + sizeof(chunk));
        new_chunk->next = NULL;
        new_chunk->prev = NULL;
        new_chunk->buffer = (linkedList->buffer + size);
        linkedList = new_chunk;
        return new_chunk->buffer;
    }
    else
    {
        // never runs, but the line above runs only once... literally not possible
        chunk *ptr = linkedList;
        printf("\n");

        while (ptr->next != NULL && first != 1)
        {
            ptr = ptr->next;
            // runs this line once and then crashes.
        }
        new_chunk->prev = ptr;
        new_chunk->size = (size + sizeof(chunk));
        new_chunk->next = NULL;
        new_chunk->buffer = (ptr->buffer + ptr->size + sizeof(chunk));
        ptr->next = new_chunk;
        // this is saying that ptr->next = 0

        // fix the insertion and you might have a good program new_chunk = ptr;
    }

    return new_chunk->buffer;
}

void myfree(void *ptr, char *file, int line)
{
}
