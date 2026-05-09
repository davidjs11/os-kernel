#   arch/aarch64/platform/pc/platform.mk   #

# Check required tools.
REQUIRED_TOOLS = $(CC) $(AS) $(LD) parted mformat mmd mcopy $(QEMU)
$(foreach tool,$(REQUIRED_TOOLS),\
    $(if $(shell command -v $(tool) 2>/dev/null),,\
        $(error Required tool '$(tool)' not found in PATH)))

# Build bootable image file
$(IMAGE): $(TARGET)
	@echo "Building kernel image file..."
	@dd if=/dev/zero of=$@ bs=1M count=64 $(NOSTDERR)
	@parted $@ mklabel gpt mkpart ESP fat32 2048s 100% set 1 esp on $(NOSTDERR)
	@mformat -i $@@@1M ::
	@mmd -i $@@@1M ::/EFI ::/EFI/BOOT ::/boot
	@mcopy -i $@@@1M $(TARGET) ::/boot/
	@mcopy -i $@@@1M arch/$(ARCH)/platform/$(PLATFORM)/limine.conf ::/boot/
	@mcopy -i $@@@1M external/limine/BOOTAA64.EFI ::/EFI/BOOT/

# Emulate
.PHONY: run
run: $(IMAGE)
	@$(QEMU)										  \
		-machine virt,gic-version=2                   \
		-cpu cortex-a72								  \
		-m 256M										  \
		-bios /usr/share/qemu-efi-aarch64/QEMU_EFI.fd \
		-drive file=$(IMAGE),format=raw 			  \
		-serial stdio								  \
		-display none   							  \
		-no-reboot      							  \
		$(NOSTDERR)

# Debug
.PHONY: debug
debug: $(IMAGE)
	@$(QEMU)										  \
		-machine virt,gic-version=2                   \
		-cpu cortex-a72								  \
		-m 256M										  \
		-bios /usr/share/qemu-efi-aarch64/QEMU_EFI.fd \
		-drive file=$(IMAGE),format=raw 			  \
		-serial none 								  \
		-display none   							  \
		-no-reboot      							  \
		-s -S &
	@$(GDB) $(TARGET)								\
		-ex "target remote localhost:1234"			\
		-ex "layout src"
