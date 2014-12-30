FILE=gpio
obj-m += $(FILE).o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insmod:
	sudo insmod ./$(FILE).ko
run:
	dmesg
rmmod:
	sudo rmmod $(FILE)	
clean:
	rm *.ko *.o
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


