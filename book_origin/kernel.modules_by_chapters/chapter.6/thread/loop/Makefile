CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
CC = gcc -Wall
EXTRA_CFLAGS += -std=gnu99

obj-m  := mod_loop.o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* *.mod
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko mcat
