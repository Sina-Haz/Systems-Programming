CFLAGS = -g -Wall -Werror -fsanitize=address

all:mymalloc
	
mymalloc:mymalloc2.c
	gcc $(CFLAGS) mymalloc2.c -o mymalloc
clean:
	rm mymalloc