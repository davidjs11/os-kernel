#   Dockerfile   #

# Build and run environment for both architectures.
#
#     docker build -t tfg-kernel .
#     docker run --rm -v "$PWD":/kernel tfg-kernel make        # Build.
#     docker run --rm -it -v "$PWD":/kernel tfg-kernel make run # Build and run.
#
# The kernel is freestanding, so Debian's native gcc is used for x86_64 and the
# aarch64-linux-gnu cross toolchain for AArch64.

FROM debian:stable-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential                                          \
        gcc-aarch64-linux-gnu                                    \
        binutils-aarch64-linux-gnu                               \
        qemu-system-x86                                          \
        qemu-system-arm                                          \
        qemu-efi-aarch64                                         \
        ovmf                                                     \
        xorriso                                                  \
        parted                                                   \
        mtools                                                   \
        gdb-multiarch                                            \
        git                                                      \
        make                                                     \
        ca-certificates                                          \
    && rm -rf /var/lib/apt/lists/*

# Build system expects *-elf-* tool names.
RUN set -e;                                                                  \
    for t in gcc as ld objcopy; do                                           \
        ln -s /usr/bin/$t /usr/local/bin/x86_64-elf-$t;                      \
        ln -s /usr/bin/aarch64-linux-gnu-$t /usr/local/bin/aarch64-elf-$t;   \
    done;                                                                    \
    ln -s /usr/bin/gdb-multiarch /usr/local/bin/x86_64-elf-gdb;              \
    ln -s /usr/bin/gdb-multiarch /usr/local/bin/aarch64-elf-gdb

WORKDIR /kernel

CMD ["make"]
