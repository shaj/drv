
CURRENT=$(shell uname -r)
KDIR=/lib/modules/$(CURRENT)/build
PWD=$(shell pwd)
DEST=/lib/modules/$(CURRENT)/misc
TARGET=ioctl_dev
obj-m:=$(TARGET).o

default:
	$(MAKE) -Og -C $(KDIR) M=$(PWD) modules
	@echo $(MAKE)
	$(CC) -o ioctl_test ioctl.c

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

