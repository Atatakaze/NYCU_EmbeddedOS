CXX = gcc

all: game guess

%.o: %.c # 可以對應所有.o和.c，例如:lab5.o對應到lab5.c
	$(CXX) -c $< 
	# $<代表第一個依賴項%.c

game: game.o
	$(CXX) -o $@ $^
	# $^代表所有依賴項，不過此處只有hw3.o

guess: guess.o
	$(CXX) -o $@ $^

clean:
	rm -rf game game.o guess guess.o

.PHONY: all clean #代表這些任務一定要執行