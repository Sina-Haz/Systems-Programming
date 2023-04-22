CFLAGS = -g -Wall -Werror -fsanitize=address

all:mysh

mysh: mysh.c 
	gcc $(CFLAGS) mysh.c -o mysh

test: testProgram.c
	gcc $(CFLAGS) testProgram.c -o test

clean:
	rm mysh
