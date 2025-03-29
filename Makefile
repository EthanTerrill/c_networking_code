CC = gcc
FLAGS = -g -lssl -lcrypto -Wall -std=gnu17

all: LinkedList.o file_system.o web_str.o HTTP_request_parser.o HTTP_response.o main.o
	$(CC) -D_GNU_SOURCE LinkedList.c file_system.c HTTP_request_parser.c HTTP_response.c web_str.c main.c -o main $(FLAGS)
	$(CC) -o client client_code.c $(FLAGS)
	$(CC) -o close_server close_server.c $(FLAGS)
	$(CC) LinkedList.o -o LL_test tests/LinkedList_test.c $(FLAGS)
	$(CC) -D_GNU_SOURCE LinkedList.c file_system.c HTTP_request_parser.c HTTP_response.c web_str.c http.c -o http $(FLAGS)
main:
	$(CC) -c main.c -o /bin/main.o $(FLAGS) 

client:
	$(CC) -o client client_code.c $(FLAGS)

web_str: web_str.c web_str.h
	$(CC) -c web_str.c $(FLAGS)

HTTP_request_parser: HTTP_request_parser.h HTTP_request_parser.c 
	$(CC) -c HTTP_request_parser.c $(FLAGS)

HTTP_response: HTTP_response.h HTTP_response.c
	$(CC) -c HTTP_response.c $(FLAGS)

LinkedList: LinkedList.h LinkedList.c
	$(CC) -c LinkedList.c $(FLAGS)

file_system: LinkedList.o LinkedList.h file_system.h
	$(CC) -c file_system.c $(FLAGS)

clean:
	rm -rf *.o *.out main client close_server LL_test
