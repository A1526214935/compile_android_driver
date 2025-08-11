MODULE_NAME := LGHUB
RESMAN_CORE_OBJS:=GAME_SJZXD.o
RESMAN_GLUE_OBJS:=process.o LG_Read.o
	$(MODULE_NAME)-objs:=$(RESMAN_CORE_OBJS) $(RESMAN_GLUE_OBJS)
	obj-m := LGHUB.o
KER=/root/777/out


all:
	make -C $(KER) M=$(PWD) ARCH=arm64 SUBARCH=arm64 modules
clean:
	@rm -rf *.ko
	@rm -rf *.mod* *.sym* .*.cmd *.order *.o