obj-m += chall.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL_PATH := ../linux-5.8.1/
all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
