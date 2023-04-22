CFLAGS = -g -Wall -Werror -fsanitize=address

all:ttts

ttts: ttts.c 
	gcc $(CFLAGS) ttts.c -o ttts

message: message.c
	gcc $(CFLAGS) message.c -o message

clean_t:
	rm ttts

clean_m:
	rm message
