#   Makefile   #

# Build-system configuration file
include config.mk

all: $(IMAGE)

# Architecture and platform specific macros.
include arch/$(ARCH)/arch.mk
include arch/$(ARCH)/platform/$(PLATFORM)/platform.mk

# Input source files and output object files.
SUBDIRS					= arch/$(ARCH)                      \
						  arch/$(ARCH)/platform/$(PLATFORM) \
						  kernel                            \
						  kernel/*   		                \
						  kernel/**/*
SOURCES_C				= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
SOURCES_ASM				= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.S))
INCLUDE					= -Iinclude -Iarch/$(ARCH)/include
OBJECTS					= $(addprefix $(BUILD)/,            \
						      $(SOURCES_C:.c=.c.$(ARCH).o)  \
							  $(SOURCES_ASM:.S=.S.$(ARCH).o))
PLATFORM_LD				= arch/$(ARCH)/platform/$(PLATFORM)/linker.ld

# Header dependencies.
DEPENDENCIES			= $(OBJECTS:.o=.d)
-include $(DEPENDENCIES)

# Target-independent flags.
CFLAGS					+=	\
	-ffreestanding			\
	-fno-stack-protector	\
	-fno-pic				\
	-fno-pie				\
	-nostdlib				\
	-std=c99				\
	-MMD					\
	-MP						\

# Debug-only flags.
ifdef DEBUG
	CFLAGS += -g -Og -DDEBUG -O0
else
	CFLAGS += -O2
endif

ifdef VERBOSE
	NOSTDERR = 
else
	NOSTDERR = 2> /dev/null
endif

# C source files.
$(BUILD)/%.c.$(ARCH).o: %.c
	@mkdir -p $(dir $@)
	@echo "CC - $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

# Assembly source files.
$(BUILD)/%.S.$(ARCH).o: %.S
	@mkdir -p $(dir $@)
	@echo "AS - $<"
	@$(AS) $(ASFLAGS) -c $< -o $@ $(INCLUDE)

# Link target.
$(TARGET): $(OBJECTS) external/limine/limine
	@echo "LD - $@"
	@$(LD) $(LDFLAGS) -T $(PLATFORM_LD) -o $@ $(OBJECTS)

# --- Miscellaneous rules --- #
# Download Limine.
external/limine/limine:
	@git clone https://codeberg.org/Limine/limine.git external/limine \
		--branch=v10.x-binary --depth=1
	@$(MAKE) -C external/limine

# Cleaning rules
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD) $(TARGET) $(IMAGE) *.elf *.iso *.img

.PHONY: clean-all
clean-all: clean
	@echo "Cleaning external tools..."
	@rm -rf external

# Help
.PHONY: help
help:
	@echo "Available architectures:"
	@echo "    - x86_64 (Intel 64)."
	@echo "    - aarch64 (ARMv8)."
	@echo "Type \`make help-<arch>\` for listing available platforms."
	@echo
