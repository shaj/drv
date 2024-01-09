CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
#CC = gcc -Wall
#KCFLAGS = gcc -noWall
#LLVM=1

obj-m := mod_rct.o
#obj-m += mod_kct.o
#obj-m += mod_koes.o
obj-m += mod_rctk.o

#ccflags-y := -w

all: default clean

default: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* .*.d *.mod
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
