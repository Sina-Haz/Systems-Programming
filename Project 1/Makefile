CFLAGS = -g -Wall -Werror -fsanitize=address

all:memgrind
	
mymalloc:mymalloc2.c
	gcc $(CFLAGS) mymalloc2.c -o mymalloc
memgrind:memgrind.c
	gcc $(CFLAGS) memgrind.c -o memgrind
clean:
	rm memgrind
	rm mymalloc