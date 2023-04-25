CFLAGS = -g -Wall -Werror -fsanitize=address

all:ttts

ttts: ttts.c 
	gcc $(CFLAGS) ttts.c -o ttts

message: message.c
	gcc $(CFLAGS) message.c -o message

client: simple_client.c
	gcc $(CFLAGS) simple_client.c -o simple_client

test: tester.c
	gcc $(CFLAGS) tester.c -o tst
clean_t:
	rm ttts

clean_m:
	rm message
