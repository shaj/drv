
CURRENT=$(shell uname -r)
KDIR=/lib/modules/$(CURRENT)/build
PWD=$(shell pwd)
DEST=/lib/modules/$(CURRENT)/misc
TARGET=dyndev
obj-m:=$(TARGET).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	@echo $(MAKE)

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

