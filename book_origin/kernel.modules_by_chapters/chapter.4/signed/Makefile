CONFIG_MODULE_SIG=n

CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
TARGET = hello_printk

obj-m      := $(TARGET).o

all: default clean
#all: default

default: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:	
	cp -v $(TARGET).ko $(DEST)
#	/sbin/depmod -v | grep $(TARGET)
	/sbin/depmod 
	/sbin/insmod $(TARGET).ko
	/sbin/lsmod | grep $(TARGET)

uninstall:	
	/sbin/rmmod $(TARGET)
	rm -v $(DEST)/$(TARGET).ko
	/sbin/depmod

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* *.mod
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
