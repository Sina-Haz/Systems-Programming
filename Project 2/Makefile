CFLAGS = -g -Wall -Werror -fsanitize=address

all:mysh

mysh: mysh.c 
	gcc $(CFLAGS) mysh.c -o mysh

clean:
	rm mysh