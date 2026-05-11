#   arch/x86_64/platform/pc/platform.mk   #

# Check required tools.
REQUIRED_TOOLS = $(CC) $(AS) $(LD) xorriso $(QEMU)
$(foreach tool,$(REQUIRED_TOOLS),\
    $(if $(shell command -v $(tool) 2>/dev/null),,\
        $(error Required tool '$(tool)' not found in PATH)))

# Build ISO file
$(IMAGE): $(TARGET)
	@echo "Building ISO file..."
	@mkdir -p										  \
		$(BUILD)/iso_root							  \
		$(BUILD)/iso_root/boot/						  \
		$(BUILD)/iso_root/boot/limine/				  \
		$(BUILD)/iso_root/EFI/						  \
		$(BUILD)/iso_root/EFI/BOOT/
	@cp												  \
		$<					  						  \
		$(BUILD)/iso_root/boot/
	@cp												  \
		arch/$(ARCH)/platform/$(PLATFORM)/limine.conf \
		external/limine/limine-bios.sys				  \
		external/limine/limine-bios-cd.bin			  \
		external/limine/limine-uefi-cd.bin			  \
		$(BUILD)/iso_root/boot/limine/
	@cp												  \
		external/limine/BOOTX64.EFI					  \
		$(BUILD)/iso_root/EFI/BOOT/
	@xorriso										  \
		-as mkisofs									  \
		-R											  \
		-r											  \
		-J											  \
		-b boot/limine/limine-bios-cd.bin			  \
		-no-emul-boot								  \
		-boot-load-size 4							  \
		-boot-info-table							  \
		-hfsplus									  \
		-apm-block-size 2048						  \
		--efi-boot boot/limine/limine-uefi-cd.bin	  \
		-efi-boot-part								  \
		--efi-boot-image							  \
		--protective-msdos-label					  \
		$(BUILD)/iso_root -o $@ $(NOSTDERR)
	@./external/limine/limine						  \
		bios-install $@ $(NOSTDERR)

# Emulate
.PHONY: run
run: $(IMAGE)
	@$(QEMU)										  \
		-cdrom $(IMAGE)								  \
		-m 256M										  \
		-serial stdio								  \
		$(NOSTDERR)

# -display none   							  \

# Debug
.PHONY: debug
debug: $(IMAGE)
	@$(QEMU)										  \
		-cdrom $(IMAGE)								  \
		-m 256M										  \
		-serial stdio								  \
		-display none								  \
		-s -S &
	@$(GDB) $(TARGET)								  \
		-ex "target remote localhost:1234"			  \
		-ex "layout src"
