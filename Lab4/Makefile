CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
FLAGS = -g -Wall --static

XLNX_DIR = /home/ding/linux-xlnx
obj-m = mydev.o

all: mydev writer reader

mydev:
	make -C $(XLNX_DIR) M=$(PWD) modules

%.o: %.cpp
	$(CXX) -c $< $(FLAGS)

writer: writer.o
	$(CXX) -o $@ $^ $(FLAGS)

reader: reader.o
	$(CXX) -o $@ $^ $(FLAGS)

clean:
	make -C $(XLNX_DIR) M=$(PWD) clean
	rm -rf writer writer.o
	rm -rf reader reader.o

.PYONY: all mydev clean