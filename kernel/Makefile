obj-m := fastafs.o
fastafs-objs := super.o

CFLAGS_super.o := -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


test:
	sudo dmesg -C
	sudo insmod fastafs.ko
	sudo rmmod fastafs.ko
	/sbin/modinfo fastafs.ko
	dmesg
