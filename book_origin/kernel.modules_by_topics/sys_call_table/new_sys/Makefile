CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
ccflags-y  += -std=gnu11

#obj-m := mod_wrc.o 
obj-m += mod_wrchg.o

all: default clean 

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ *.mod
	@rm -fR .tmp* .*.d 
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
