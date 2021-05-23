export ARCH := x86_64
export AS := nasm
export CC := gcc
export LD := ld

SCRIPTS := $(CURDIR)/scripts
ETC := $(CURDIR)/etc
SRC := $(CURDIR)/src
export MODULES := $(CURDIR)/modules

TARGETS = boot kernel

all: image

image: $(TARGETS)
	dd if=/dev/zero of=image bs=512 count=93750
	mformat -i image -f 1440 ::
	mmd -i image ::/EFI
	mmd -i image ::/EFI/BOOT
	mcopy -i image $(SRC)/boot/boot.efi ::/EFI/BOOT
	mcopy -i image $(SCRIPTS)/startup.nsh ::
	mcopy -i image $(SRC)/kernel/kernel.elf ::
	mcopy -i image $(ETC)/zap-light16.psf ::

PHONY += $(TARGETS)
$(TARGETS):
	$(MAKE) -C $(SRC)/$@

PHONY += run
run: image
	qemu-system-x86_64 -m 2G -cpu qemu64 -net none \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/qemu/OVMF.fd,readonly=on \
		-drive format=raw,file=image

PHONY += clean
clean:
	@for TARGET in $(TARGETS); \
	do \
		$(MAKE) -C $(SRC)/$${TARGET} clean; \
	done
	rm -f image

.PHONY: $(TARGET)
