obj-m += lab4_driver.o

KDIR = ../

ARCH_NAME = arm64
TOOL_CHAIN = aarch64-linux-gnu-

all: lab4_driver.c
	sudo make ARCH=$(ARCH_NAME) CROSS_COMPILE=$(TOOL_CHAIN) -C $(KDIR) M=$(shell pwd) modules

writer: writer.c
	$(TOOL_CHAIN)gcc -o writer writer.c

reader: reader.c
	$(TOOL_CHAIN)gcc -o reader reader.c

clean:
	sudo make ARCH=$(ARCH_NAME) CROSS_COMPILE=$(TOOL_CHAIN) -C $(KDIR) M=$(shell pwd) clean
	@rm writer
	@rm reader


.PHONY: all clean
