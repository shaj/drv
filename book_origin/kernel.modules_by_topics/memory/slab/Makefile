CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

TARGET1 = slab
TARGET2 = pool

obj-m      := $(TARGET1).o $(TARGET2).o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order *.mod
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp* 
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
