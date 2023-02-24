CFLAGS = -g -Wall -Werror -fsanitize=address

all: memgrind
	
mymalloc:mymalloc.c
	gcc $(CFLAGS) mymalloc.c -o mymalloc
memgrind:memgrind.c
	gcc $(CFLAGS) memgrind.c -o memgrind
test1:test1.c
	gcc $(CFLAGS) test1.c -o test1
test2:test2.c
	gcc $(CFLAGS) test2.c -o test2
test3:test3.c
	gcc $(CFLAGS) test3.c -o test3
clean:
	rm memgrind
	rm mymalloc
	rm test1
	rm test2
	rm test3