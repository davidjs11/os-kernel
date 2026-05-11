#   config.mk   #

# Build-system configuration file.

# x86_64 + PC
ARCH					:= x86_64
PLATFORM				:= pc
IMAGE                   := kernel.iso

# # AArch64 + QEMU virt
# ARCH					:= aarch64
# PLATFORM				:= qemu-virt
# IMAGE                   := kernel.img

BUILD					?= ./build
TARGET				 	:= $(BUILD)/kernel-$(ARCH)-$(PLATFORM).elf
DEBUG					:= 1
