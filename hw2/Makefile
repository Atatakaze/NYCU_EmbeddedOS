CC=gcc
CFLAGS=-g

all: hw2

hw2: hw2.o socket_utils.o
	$(CC) $(CFLAGS) $^ -o hw2

hw2.o: hw2.c
	$(CC) $(CFLAGS) -c $^

socket_utils.o: socket_utils.c
	$(CC) $(CFLAGS) -c $^

clean:
	@rm hw2 hw2.o socket_utils.o

.PHONY: all clean