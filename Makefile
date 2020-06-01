obj-m += ouichefs.o mod_does_nothing.o mod_remove_bigger.o
ouichefs-objs := fs.o super.o inode.o file.o dir.o ex1.o ex2_1.o ex2_2.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNELDIR) M=$(PWD) modules

debug:
	make -C $(KERNELDIR) M=$(PWD) ccflags-y+="-DDEBUG -g" modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -rf *~

.PHONY: all clean
