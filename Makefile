CC = gcc
FLAGS = -g -Wall -std=c17

all: 
	$(CC) -o main main.c $(FLAGS)
	$(CC) -o client client_code.c $(FLAGS)

main:
	$(CC) -o main main.c $(FLAGS) 

client:
	$(CC) -o client client_code.c $(FLAGS)

clean:
	rm -rf *.o *.out main client
