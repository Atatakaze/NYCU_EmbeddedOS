CC=gcc
CFLAGS=-Wall -g
ING_FLAGS=-lpthread

hw3: hw3.o socket_utils.o
	$(CC) hw3.o socket_utils.o $(CFLAGS) $(ING_FLAGS) -o hw3

server: server.o socket_utils.o
	${CC} server.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o server

client: client.o socket_utils.o
	${CC} client.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o client

all: server.o client.o socket_utils.o
	${CC} client.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o client
	${CC} server.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o server

hw3.o: hw3.c
	$(CC) $(CFLAGS) -o hw3.o -c hw3.c

server.o: server.c
	$(CC) $(CFLAGS) -o server.o -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -o client.o -c client.c

socket_utils.o:
	$(CC) $(CFLAGS) -o socket_utils.o -c socket_utils.c

clean: 
	${RM} server client server.o client.o socket_utils.o

.PHONY: all clean