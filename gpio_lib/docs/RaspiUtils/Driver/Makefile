obj-m := raspiutil.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)


.phony: driver
driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules


.phony: clean	
clean:
	rm -f ./raspiutil.ko
	rm -f ./.raspiutil.ko.cmd
	rm -f ./raspiutil.mod.c
	rm -f ./raspiutil.mod.o
	rm -f ./.raspiutil.mod.o.cmd
	rm -f ./raspiutil.o
	rm -f ./.raspiutil.o.cmd
	rm -f ./modules.order
	rm -f ./Module.symvers
	rm -f ./.tmp_versions/raspiutil.mod
	rmdir ./.tmp_versions


.phony: install
install:
	-rmmod raspiutil
	chmod 666 ./raspiutil.ko
	cp ./raspiutil.ko /lib/modules/$(shell uname -r)/kernel/drivers/misc/raspiutil.ko
	depmod
	modprobe raspiutil
	chmod 666 /dev/raspi0
	dmesg | tail -20
