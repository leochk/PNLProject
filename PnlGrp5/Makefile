obj-m += ouichefs.o src_mod/mod_does_nothing/mod_does_nothing.o src_mod/mod_remove_bigger/mod_remove_bigger.o
ouichefs-objs := src_fs/fs.o src_fs/super.o src_fs/inode.o src_fs/file.o src_fs/dir.o src_fs/ex1.o src_fs/ex2_1.o src_fs/ex2_2.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules

debug:
	make -C $(KERNELDIR) M=$(PWD) ccflags-y+="-DDEBUG -g" modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -rf *~

.PHONY: all clean
