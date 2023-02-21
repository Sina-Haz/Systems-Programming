#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

//metadata includes: the size of the data, whether its free or not, a ptr to the data.
typedef struct header{
    int payload_size;
    short isValid;
    void* ptr;
}header;

#define HEADER_SIZE sizeof(header)
#define HEAP_SIZE 4096
static char heap[HEAP_SIZE] = {0};

void *mymalloc(size_t size, char *file, int line);
void myfree(void *ptr, char *file, int line);

//initializes heap with 1 header and sets the chunk to 0 with memset
void initializeMemory(){
    header* h = (header*) heap;
    h->isValid = 0;
    h->payload_size = HEAP_SIZE-HEADER_SIZE;
    h->ptr = heap + HEADER_SIZE;
    memset(h->ptr,0,h->payload_size);
}

/*
    cases: 
    case 1 data is free and chunk is big enough-->break it up into 2 chunks need a new header for new chunk
    case 2: data is free, chunk too small.(eager coalescing) Go to next chunk look for case 1 scenario.
    case 3:data not free, go to next chunk
    case 4: we've reached the end of heap, return NULL
*/
void *mymalloc(size_t size, char *file, int line){
    if(*heap == (char)0){
        initializeMemory();
    }

    int index = 0;
    do
    {
        header* h = (header*) heap + index;

        if(h->isValid == 0){
            if(size <= h->payload_size){
                //if size is such that we can split up this chunk into 2 chunks, need to make a new header
                if(size < (h->payload_size - HEADER_SIZE)){
                    header *new = h + HEADER_SIZE + size;
                    new->isValid = 0;
                    new->payload_size = h->payload_size - size - HEADER_SIZE;
                    new->ptr = new + HEADER_SIZE;
                    h->payload_size = size;
                }
                //if the size is too large to split up this chunk
                h->isValid = 1;
                return h->ptr;
            }
        }
        //if h is free but not large enough, or if its not free, go to next chunk, add to index.
        index+= HEADER_SIZE + h->payload_size;

    } while(index < HEAP_SIZE);
    printf("There was not enough memory to allocate for your object.");
    return NULL;

}

//make sure to implement eager coalescing here, so once we free something make sure things adjacent 
//(either behind or in front) that are also free get merged into 1 chunk.
void myfree(void *ptr, char *file, int line){
    //first we iterate through the memory to make sure that the pointer was malloced and that its not freed.
    int index = 0;
    int isPtrMalloced = 0;
    do{
        header* h = (header*) heap + index;
        if(ptr == h->ptr){
            if(h->isValid == 1){
                isPtrMalloced++;
            }
            else{
                printf("Given pointer was already freed.\n");
                return;
            }
            break;
        }
        index+=HEADER_SIZE+h->payload_size;
    }while(index < HEAP_SIZE);
    if(isPtrMalloced == 0){
        if(ptr > (void*) heap && ptr <= (void*) (heap+HEAP_SIZE)){
            printf("given address is not at the start of a chunk of allocated memory\n");
        }
        else{
            printf("Given pointer was not allocated with malloc.\n");
        }
        return;
    }

    //now we free the memory. If chunk ahead is also free we coalesce them.
    header* h = (header*) heap+index;
    h->isValid = 0;
    

    //before we eager coalesce need to make sure that this isn't the last chunk in the heap.
    //can likely do this with index
    if(index+HEADER_SIZE+h->payload_size < HEAP_SIZE){
        header* next = (header*) ptr + h->payload_size;
        if(next -> isValid == 0){
            h -> payload_size += HEADER_SIZE + next -> payload_size;
        }
    }

}

void printMem(){
    int index = 0;
    do{
        header* h = (header*) heap+index;
        printf("Chunk at index: %d has size %d. Valid value: %d\n",index,h->payload_size,h->isValid);
        index+=HEADER_SIZE+h->payload_size;
    }while(index < HEAP_SIZE);
}


int main(){
    int* arr = malloc(5*sizeof(int));

    char* string = malloc(45*sizeof(char));

    string = memcpy(string,"Hello my name is Sina Hazeghi",30);

    printMem();

    free(string);

    printMem();

    int num = 0;
    
    free(&num);

    free(arr+2);

    free(arr);

    return EXIT_SUCCESS;

}