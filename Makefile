#   Makefile   #

ARCH					?= x86_64
PLATFORM				?= pc
BUILD					?= ./build
TARGET					?= $(BUILD)/kernel.elf
IMAGE					?= kernel.iso

# Check required tools.
REQUIRED_TOOLS = $(CC) $(AS) $(LD) xorriso $(QEMU)
$(foreach tool,$(REQUIRED_TOOLS),\
    $(if $(shell command -v $(tool) 2>/dev/null),,\
        $(error Required tool '$(tool)' not found in PATH)))

all: $(IMAGE)

# Architecture and platform specific macros.
include arch/$(ARCH)/arch.mk
include arch/$(ARCH)/platform/$(PLATFORM)/platform.mk

# Build-system configuration file
include config.mk

# Input source files and output object files.
SUBDIRS					= arch/$(ARCH) arch/$(ARCH)/platform/$(PLATFORM) kernel
SOURCES_C				= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
SOURCES_ASM				= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.S))
INCLUDE					= include
OBJECTS					= $(addprefix $(BUILD)/,$(SOURCES_C:.c=.c.o) $(SOURCES_ASM:.S=.S.o))
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
	-I$(INCLUDE)

# Debug-only flags.
ifdef DEBUG
	CFLAGS += -g -Og -DDEBUG
else
	CFLAGS += -O2
endif

ifdef VERBOSE
	NOSTDERR = 
else
	NOSTDERR = 2> /dev/null
endif

# C source files.
$(BUILD)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC - $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assembly source files.
$(BUILD)/%.S.o: %.S
	@mkdir -p $(dir $@)
	@echo "AS - $<"
	@$(AS) $(ASFLAGS) -c $< -o $@

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
	@rm -rf $(BUILD) $(TARGET) $(IMAGE)

.PHONY: clean-all
clean-all: clean
	@echo "Cleaning external tools..."
	@rm -rf external

# Help
.PHONY: help
help:
	@echo "Available architectures:"
	@echo "    - x86_64"
	@echo "Type \`make help-<arch>\` for listing available platforms."
	@echo
