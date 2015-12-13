obj-m := cipherdev.o
cipherdev-objs := cipherdev_main.o


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default: all

all: build cipherctl test ioctl

build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

cipherctl: 
	gcc -g -Wall cipherctl.c -o cipherctl.o

test:
	gcc -g -Wall test.c -o test.o

ioctl:
	gcc -g -Wall ioctl.c -o ioctl.o

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.* modules.*
