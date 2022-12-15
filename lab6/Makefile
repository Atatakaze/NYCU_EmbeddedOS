CXX = gcc

all: server client

server:
	$(CXX) -o server server.c -lpthread

client:
	$(CXX) -o client client.c

clean:
	rm -rf server client

.PHONY: all clean