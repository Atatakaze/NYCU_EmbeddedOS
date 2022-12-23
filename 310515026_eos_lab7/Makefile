CC = gcc

all: game guess

%.o: %.c
	$(CC) -c $< 

game: game.o
	$(CC) -o $@ $^

guess: guess.o
	$(CC) -o $@ $^

clean:
	rm -rf game game.o guess guess.o

.PHONY: all clean