CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
# https://ask-dev.ru/info/403420/kernel-module-compilation-and-kbuildnopedantic
EXTRA_CFLAGS += -std=gnu11
ccflags-y  += $(EXTRA_CFLAGS)

obj-m += mod_read.o
#obj-m += mod_write.o
#obj-m += mod_vfs.o
#obj-m += mdw.o

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
