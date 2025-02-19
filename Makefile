CC = gcc
FLAGS = -g -Wall -std=c17

all: web_str.o 
	$(CC) web_str.o -o main main.c $(FLAGS)
	$(CC) -o client client_code.c $(FLAGS)

main:
	$(CC) -o main main.c $(FLAGS) 

client:
	$(CC) -o client client_code.c $(FLAGS)

web_str: web_str.c web_str.h
	$(CC) -c web_str.c $(FLAGS)

clean:
	rm -rf *.o *.out main client
