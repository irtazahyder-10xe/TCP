SHELL := /bin/bash

PREFIX := riscv64-unknown-elf
GCC := $(PREFIX)-gcc
OBJDUMP := $(PREFIX)-objdump

CFLAGS := -march=rv32g -mabi=ilp32 -T startup/link.ld -nostdlib

QEMU_PATH := /home/lpt-10xe-10/Desktop/10xAssignments/qemu/qemu
QEMU_FLAGS := --cpu=x86_64 --enable-debug

TARGETS := "riscv32-softmmu,riscv64-softmmu"
ENV_DIR := env

ASM_FILES := $(wildcard startup/*.S)
BOOT_FILE := bin/boot.elf
DTB ?=

# xxx-softmmu for system emulation
# xxx-linux-user for user emulation
build_env:
	@mkdir -p $(ENV_DIR); \
	cd $(ENV_DIR); \
	$(QEMU_PATH)/configure --target-list=$(TARGETS); \
	make -j $(nproc)

clean_env:
	@rm -rf $(ENV_DIR)/

build: $(ASM_FILES)
	@mkdir -p bin
	$(GCC) $(CFLAGS) $^ -o $(BOOT_FILE)

clean: 
	@rm *.o bin/*

run_vm: $(BOOT_FILE)
	./env/qemu-system-riscv32 \
		-M virt,aia=aplic \
		-cpu rv32,c=off \
		-bios none \
		-m maxmem=16G \
		-smp 1,cores=1,threads=1 \
		-accel tcg \
		-nographic \
		-device loader,file=./$(BOOT_FILE),addr=0x80000000

$(BOOT_FILE): build

dtb_to_dts:
ifndef DTB
	@echo "make dtb_to_dts: missing operand DTB"
	@echo "make dtb_to_dts: try 'make dtb_to_dts DTB=<dtbfile>'"
	@exit 2
endif
	@dtb_file_path=$(DTB); \
	if [ -e "$$dtb_file_path" ]; then \
		dtc -I dtb -O dts $$dtb_file_path -o $${dtb_file_path//dtb/dts}; \
	else \
		echo "make dtb_to_dts: $$dtb_file_path: No such file"; \
		exit 1; \
	fi
