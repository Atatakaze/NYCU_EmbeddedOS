CC=gcc
CFLAGS=-Wall -g

all: hw3.o socket_utils.o
	$(CC) hw3.o socket_utils.o $(CFLAGS) -o hw3

hw3.o: hw3.c
	$(CC) $(CFLAGS) -o hw3.o -c hw3.c

socket_utils.o:
	$(CC) $(CFLAGS) -o socket_utils.o -c socket_utils.c

clean: 
	${RM} socket_utils.o hw3.o hw3

.PHONY: all clean