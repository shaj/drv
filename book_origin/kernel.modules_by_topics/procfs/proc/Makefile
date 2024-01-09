CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
CC = gcc -Wall

EXTRA_CFLAGS += -std=gnu99

#TARGET1 = mod_procr
#TARGET2 = mod_procr2
#TARGET5 = mod_2

obj-m  := mod_proc.o
obj-m  += mod_proct.o
obj-m  += mod_proct.o

all: default mcat clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* *.mod
	@rm -rf .tmp_versions

disclean: clean	
	@rm -f *.ko mcat
