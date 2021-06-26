SYSLIB = /usr/lib
EFIINC = /usr/include/efi
EFILDS = $(SYSLIB)/elf_$(ARCH)_efi.lds
EFICRT = $(SYSLIB)/crt0-efi-$(ARCH).o
CFLAGS = -I$(MODULES) -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol \
		-O2 -fpie -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar \
		-mno-red-zone -maccumulate-outgoing-args -DEFI_FUNCTION_WRAPPER -Wall -Wextra -g \
		-DDEBUG_BOOT_GRAPHIC
LDFLAGS = -T$(EFILDS) -nostdlib -znocombreloc -shared -Bsymbolic -L$(SYSLIB) $(EFICRT)

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

all: boot.efi

boot.efi: boot.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* \
		-j .rela.* -j .reloc --target efi-app-$(ARCH) --subsystem=10 $< $@

boot.so: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@ -lefi -lgnuefi

%.d: %.c
	$(CC) $(CFLAGS) -MM -MT "$@ $(@:.d=.o)" $< -o $@

PHONY += clean
clean:
	rm -f boot.efi boot.so $(OBJS) $(DEPS)

include $(DEPS)

.PHONY: $(PHONY)
