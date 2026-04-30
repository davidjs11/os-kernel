#   arch/x86_64/arch.mk   #

# x86_64-specific macro definitions.

CC						= x86_64-elf-gcc
AS						= x86_64-elf-as
LD						= x86_64-elf-ld
GDB						= x86_64-elf-gdb
QEMU					= qemu-system-x86_64

CFLAGS					+=	\
    -m64                    \
    -mcmodel=kernel         \
    -mno-red-zone           \
    -mno-mmx                \
    -mno-sse                \
    -mno-sse2				\
	-Iinclude/

.PHONY: help
help-x86_64:
	@echo "Available platforms for x86_64 architecture:"
	@echo "    - pc (Generic PC)"
	@echo
