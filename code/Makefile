# 内核模块名称
MODULE_NAME := hide_proc
obj-m := $(MODULE_NAME).o

# 内核构建目录
KDIR ?= /lib/modules/$(shell uname -r)/build

# 用户空间程序名称
USER_TARGET := hide_proc_ctl

# 默认目标
all: modules $(USER_TARGET)

# 构建内核模块
modules:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

# 构建用户空间程序
$(USER_TARGET): hide_proc_ctl.c hide_proc.h
	gcc -Wall -Wextra -o $@ $<

# 清理
clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	rm -f $(USER_TARGET) modules.order Module.symvers

# 安装
install: modules $(USER_TARGET)
	sudo insmod $(MODULE_NAME).ko || true
	sudo chmod +x setup_device.sh || true
	sudo ./setup_device.sh || true
	sudo cp $(USER_TARGET) /usr/local/bin/

# 卸载
uninstall:
	sudo rmmod $(MODULE_NAME) || true
	sudo rm -f /usr/local/bin/$(USER_TARGET) || true
	sudo rm -f /dev/hide_proc || true

.PHONY: all modules clean install uninstall
