SHELL := /bin/bash

# =================== QEMU ===================
QEMU_PATH := /home/lpt-10xe-10/Desktop/10xAssignments/qemu/qemu
QEMU_FLAGS := --cpu=x86_64 --enable-debug

QEMU := ./env/qemu-system-riscv64
DOMAIN_CFG := domain-count=5,domain-mode=M_M_S_S_S,domain-parent=-1_0_0_1_1
MACHINE := -M virt,aia=aplic-imsic
ifeq ($(IDM_TEST),true)
	MACHINE := $(MACHINE),$(DOMAIN_CFG)
endif
CPUS := -cpu rv64,c=off -smp 1,cores=1,threads=1 -m maxmem=16G
ACCEL := -accel tcg
DEVICE := 
BACKEND := -serial mon:stdio
INTERFACE := -nographic
BOOT = $(DEBUG) -bios none -device loader,file=$(BOOT_FILE),addr=0x80000000

# =================== GCC ===================

PREFIX := riscv64-unknown-elf
GCC := $(PREFIX)-gcc
OBJDUMP := $(PREFIX)-objdump
RISCV_GDB := $(PREFIX)-gdb
x86_GDB := gdb

CPATH := ./include
LDFLAGS := -fdata-sections -Wl,--gc-sections
CFLAGS := -march=rv64g -mabi=lp64 -mcmodel=medany -g -I $(CPATH) \
	  -Wall -Werror -Wcast-align -Wconversion -Wrestrict -Wrestrict \
	  -Wconversion -Wsign-conversion -Wshadow -Wpointer-arith -Wcast-align \
	  -Wmissing-prototypes -g -O2 -Wimplicit-fallthrough -Wformat=2 \
	  -Wformat-security $(LDFLAGS)

TARGETS := "riscv32-softmmu,riscv64-softmmu"
ENV_DIR := env

BUILD_DIR := build
SRC_DIR := src
ASM_DIR := startup

ASM_FILES := $(wildcard $(ASM_DIR)/*.S)
C_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst startup/%.S,build/%.o,$(ASM_FILES)) \
	     $(patsubst src/%.c,build/%.o,$(C_FILES))

BOOT_FILE := bin/boot.elf
DTB ?=

# xxx-softmmu for system emulation
# xxx-linux-user for user emulation
.PHONY: help, build_env, clean_env, build, clean, run_vm, dtb_to_dts, gdb, clean
help:
	echo "For debugging: make run_vm DEBUG=\"-s -S\""

build_env:
	@mkdir -p $(ENV_DIR); \
	cd $(ENV_DIR); \
	$(QEMU_PATH)/configure --target-list=$(TARGETS); \
	make -j $(nproc)

clean_env:
	@rm -rf $(ENV_DIR)/

build_qemu:
	cd env; \
	make all -j $(nproc); \
	make man

clean_qemu:
	cd env; \
	make clean

build: $(BOOT_FILE)

clean: 
	@rm *.o bin/* build/*

disasm:
	$(OBJDUMP) -xDt $(BOOT_FILE) > misc/boot.disasm

run_vm: $(BOOT_FILE)
		$(QEMU) $(MACHINE) \
			$(CPUS) \
			$(ACCEL) \
			$(DEVICE) \
			$(BACKEND) \
			$(INTERFACE) \
			$(BOOT)
		
gdb_vm:
	# For now using the -s flag in vm to automatically connect to GDB at
	# tcp:1234
	$(RISCV_GDB) $(BOOT_FILE) -ex "target remote localhost:1234"

gdb_qemu:
	# run $(arguments) i.e. run -M virt
	$(x86_GDB) $(QEMU)

dtb_to_dts:
ifndef DTB
	@echo "make dtb_to_dts: missing operand DTB"
	@echo "make dtb_to_dts: try 'make dtb_to_dts DTB=<dtbfile>'"
	@exit 2
endif
	$(QEMU) $(MACHINE),dumpdtb=$(DTB) $(CPUS) -display none
	@dtb_file_path=$(DTB); \
	if [ -e "$$dtb_file_path" ]; then \
		dtc -I dtb -O dts $$dtb_file_path -o $${dtb_file_path//dtb/dts}; \
	else \
		echo "make dtb_to_dts: $$dtb_file_path: No such file"; \
		exit 1; \
	fi

$(BOOT_FILE): $(OBJ_FILES)
	@mkdir -p bin
	@$(GCC) $(CFLAGS) -nostdlib -T startup/link.ld $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p build
	@$(GCC) $(CFLAGS) -fno-builtin -c $< -o $@

$(BUILD_DIR)/%.o: $(ASM_DIR)/%.S
	@mkdir -p build
	@$(GCC) $(CFLAGS) -fno-builtin -c $< -o $@
