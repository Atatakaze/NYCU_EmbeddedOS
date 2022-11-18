obj-m += hw1_driver.o

KDIR = ../../

ARCH_NAME = arm64
TOOL_CHAIN = aarch64-linux-gnu-

all: hw1_driver.c
	sudo make ARCH=$(ARCH_NAME) CROSS_COMPILE=$(TOOL_CHAIN) -C $(KDIR) M=$(shell pwd) modules

hw1: hw1.c
	$(TOOL_CHAIN)gcc -o hw1 hw1.c

clean:
	sudo make ARCH=$(ARCH_NAME) CROSS_COMPILE=$(TOOL_CHAIN) -C $(KDIR) M=$(shell pwd) clean
	@rm hw1


.PHONY: all clean
