CFLAGS = -g -Wall -Werror -fsanitize=address

all:memgrind
	
mymalloc:mymalloc.c
	gcc $(CFLAGS) mymalloc.c -o mymalloc
memgrind:memgrind.c
	gcc $(CFLAGS) memgrind.c -o memgrind
clean:
	rm memgrind
	rm mymalloc