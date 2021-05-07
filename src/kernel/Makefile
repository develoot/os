SYSLIB = /usr/lib
EFIINC = /usr/include/efi
CFLAGS = -I$(LIB) -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol \
		 -Wall -Wextra -fpie -ffreestanding -fshort-wchar -g -DDEBUG_ASSERT
LDFLAGS = -static -Bsymbolic -nostdlib
ASMFLAGS =

SRCS = $(shell find . -name '*.c' -type 'f') $(shell find $(LIB) -name '*.c' -type 'f')
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

ASMS = $(shell find $(LIB) -name '*.asm' -type 'f')
ASMOBJS = $(ASMS:.asm=.o)

LDS = kernel.lds

all: kernel.elf

kernel.elf: $(LDS) $(OBJS) $(ASMOBJS)
	$(LD) -T$(LDS) $(LDFLAGS) -o $@ $(OBJS) $(ASMOBJS)

PHONY += clean
clean:
	rm -f kernel.elf $(OBJS) $(ASMOBJS) $(DEPS)

$(ASMOBJS): $(ASMS)
	$(AS) $(ASMFLAGS) -o $@ -f elf64 $^

%.d: %.c
	rm -f $@; \
	$(CC) $(CFLAGS) -MM -MT "$@ $(@:.d=.o)" $< -o $@ \

include $(DEPS)

.PHONY: $(PHONY)
