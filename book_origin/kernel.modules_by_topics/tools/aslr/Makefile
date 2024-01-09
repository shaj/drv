CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc
TARGET = aslr

obj-m      := $(TARGET).o

#all: default clean intptr

all: module clean intptr

module: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules

intptr: intptr.c
	gcc -Wall intptr.c -o intptr

clean:	
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~
	@rm -fR .tmp* *.mod intptr
	@rm -rf .tmp_versions

disclean: clean
	@rm -f *.ko
