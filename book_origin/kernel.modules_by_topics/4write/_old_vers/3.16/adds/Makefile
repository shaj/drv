CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
CC = gcc -Wall

TARGET1 = nsys
TARGET2 = adds

obj-m := $(TARGET1).o $(TARGET2).o

all:	modules syscall clean

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

syscall: syscall.h syscall.c
	$(CC) syscall.c -o syscall

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c
	@rm -f .*.*.cmd *~ *.*~ *.symvers
	@rm -fR .tmp* *.order
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko syscall
