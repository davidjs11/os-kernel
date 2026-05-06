#   arch/aarch64/arch.mk   #

# Aarch64-specific macro definitions.

CC						= aarch64-elf-gcc
AS						= aarch64-elf-as
LD						= aarch64-elf-ld
GDB						= aarch64-elf-gdb
QEMU					= qemu-system-aarch64

CFLAGS					+=	\
	-ffreestanding          \
	-Iinclude/

.PHONY: help
help-aarch64:
	@echo "Available platforms for Aarch64 architecture:"
	@echo "    - qemu-virt (QEMU Virtual Platform)"
	@echo
