CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

#obj-m += md_32c.o
obj-m += md_32o.o
#obj-m += md_64c.o
#obj-m += md_64o.c
#obj-m += md_64p.o

all: default clean

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* .*.cmd *.mod .*.d
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko

