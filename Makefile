CC = gcc
FLAGS = -Wall -std=c17

all: LinkedList.o file_system.o web_str.o HTTP_request_parser.o HTTP_response.o
	$(CC) LinkedList.o file_system.o HTTP_request_parser.o HTTP_response.o web_str.o -o main main.c $(FLAGS)
	$(CC) -o client client_code.c $(FLAGS)
	$(CC) -o close_server close_server.c $(FLAGS)
	$(CC) LinkedList.o -o LL_test tests/LinkedList_test.c $(FLAGS)

main:
	$(CC) -o main main.c $(FLAGS) 

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
