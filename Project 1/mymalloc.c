#include<stdio.h>
#define malloc(x) mymalloc(x,__FILE__,__LINE__)
#define MEMSIZE 4096

static char memory[MEMSIZE];