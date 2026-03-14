#   arch/x86_64/arch.mk   #

# x86_64-specific macro definitions.

CC						= x86_64-elf-gcc
AS						= x86_64-elf-as
LD						= x86_64-elf-ld
GDB						= x86_64-elf-gdb

CFLAGS 					:= 	\
    -mno-red-zone 			\
    -m64
