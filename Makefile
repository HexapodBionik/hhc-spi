TARGET_MODULE := hhc-spi

ifneq ($(KERNELRELEASE),)
# If the script is invoked by kernel building system

$(TARGET_MODULE)-objs := main.o fs.o spi.o
obj-m := $(TARGET_MODULE).o

else

BUILDSYSTEM_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all :
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

load:
	insmod ./$(TARGET_MODULE).ko

unload:
	rmmod ./$(TARGET_MODULE).ko

endif
