CXX = g++
CFLAG = -g -Wall -std=c++11

all: lab5

%.o: %.cpp # 可以對應所有.o和.cpp，例如:lab5.o對應到lab5.cpp
	$(CXX) -c $< $(CFLAG)
	# $<代表第一個依賴項%.cpp

lab5: lab5.o
	$(CXX) -o $@ $^ $(CFLAG)
	# $^代表所有依賴項，不過此處只有lab5.o

clean:
	rm -rf lab5 lab5.o

.PHONY: all clean #代表這些任務一定要執行