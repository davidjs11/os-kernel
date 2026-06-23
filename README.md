# Kernel for x86_64 and ARMv8

Operating system kernel written from scratch with support for the x86_64 and
AArch64 (ARMv8) architectures, developed as a Bachelor's Thesis (Trabajo de Fin
de Grado) for the Computer Engineering degree at the University of Granada.

The kernel boots with the [Limine](https://codeberg.org/Limine/limine)
bootloader and runs on QEMU. It includes physical and virtual memory
management, a kernel heap, a preemptive scheduler, user space processes, system
calls and a small user shell.

## Source tree

```
arch/        Architecture and platform specific code
device/      Device drivers (serial, IRQ, timer, framebuffer)
include/     Public kernel headers
kernel/      Common kernel code (mm, sched, syscall, tests, kmain)
user/        User space programs
external/    External dependencies (Limine, downloaded at build time)
config.mk    Architecture and platform selection
Makefile     Build system
```

## Requirements

- Cross toolchain: `x86_64-elf-{gcc,as,ld,objcopy}` or
  `aarch64-elf-{gcc,as,ld,objcopy}`.
- `qemu-system-x86_64` or `qemu-system-aarch64`.
- x86_64: `xorriso`.
- AArch64: `parted`, `mtools` and the `qemu-efi-aarch64` firmware.

## Building and running

The architecture and platform are selected in `config.mk`. The default is
x86_64 + PC; for AArch64 + QEMU virt, comment out the first block and uncomment
the second one.

```bash
make          # Build the bootable image
make run      # Build and run on QEMU
make debug    # Run on QEMU with GDB attached
make clean    # Remove build artifacts
make help     # Show the available architectures
```

The kernel output and the user shell go through the serial port, in the same
terminal. With `DEBUG` enabled in `config.mk`, the kernel tests also run during
boot.

## Docker

The `Dockerfile` includes all the dependencies, so the toolchain doesn't need
to be installed on the host:

```bash
docker build -t tfg-kernel .
docker run --rm -v "$PWD":/kernel tfg-kernel make          # Build
docker run --rm -it -v "$PWD":/kernel tfg-kernel make run  # Build and run
```
