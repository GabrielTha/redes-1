all: client server
client: 
		gcc -w client.c -o client.out -lncurses

server:
		gcc -w server.c -o server.out -lncurses