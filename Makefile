CC = gcc
FLAGS = -g -Wall -std=c17

all: web_str.o HTTP_request_parser.o
	$(CC) HTTP_request_parser.o web_str.o -o main main.c $(FLAGS)
	$(CC) -o client client_code.c $(FLAGS)
	$(CC) -o close_server close_server.c $(FLAGS)

main:
	$(CC) -o main main.c $(FLAGS) 

client:
	$(CC) -o client client_code.c $(FLAGS)

web_str: web_str.c web_str.h
	$(CC) -c web_str.c $(FLAGS)

HTTP_request_parser: HTTP_request_parser.h HTTP_request_parser.c 
	$(CC) -c HTTP_request_parser.c $(FLAGS)

clean:
	rm -rf *.o *.out main client close_server
