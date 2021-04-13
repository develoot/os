export ARCH := x86_64
export CC := gcc
export LD := ld
export SRC := $(CURDIR)/src

SCRIPTS := $(CURDIR)/scripts
ETC := $(CURDIR)/etc
CORE := $(CURDIR)/core

TARGETS = boot kernel

all: image

image: $(TARGETS)
	dd if=/dev/zero of=image bs=512 count=93750
	mformat -i image -f 1440 ::
	mmd -i image ::/EFI
	mmd -i image ::/EFI/BOOT
	mcopy -i image $(CORE)/boot/boot.efi ::/EFI/BOOT
	mcopy -i image $(SCRIPTS)/startup.nsh ::
	mcopy -i image $(CORE)/kernel/kernel.elf ::
	mcopy -i image $(ETC)/zap-light16.psf ::

PHONY += $(TARGETS)
$(TARGETS):
	$(MAKE) -C $(CORE)/$@

PHONY += run
run: image
	qemu-system-x86_64 -m 256M -cpu qemu64 -net none \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/qemu/OVMF.fd,readonly=on \
		-drive format=raw,file=image

PHONY += clean
clean:
	@for TARGET in $(TARGETS); \
	do \
		$(MAKE) -C $(CORE)/$${TARGET} clean; \
	done
	rm -f image

.PHONY: $(TARGET)
