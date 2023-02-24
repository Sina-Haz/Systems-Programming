CFLAGS = -g -Wall -Werror -fsanitize=address

all: memgrind
	
mymalloc:mymalloc.c
	gcc $(CFLAGS) mymalloc.c -o mymalloc
memgrind:noahsMemgrind.c
	gcc $(CFLAGS) noahsMemgrind.c -o memgrind
clean:
	rm memgrind
	rm mymalloc