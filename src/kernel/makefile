SYSLIB = /usr/lib
EFIINC = /usr/include/efi
CFLAGS = -I$(MODULES) -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol \
		 -Wall -Wextra -fpie -ffreestanding -fshort-wchar -g -DDEBUG_ASSERT
LDFLAGS = -Bstatic -Bsymbolic -pie -nostdlib --gc-sections --print-gc-sections --no-undefined-version
ASMFLAGS =

SRCS = $(shell find . -name '*.c' -type 'f') $(shell find $(MODULES) -name '*.c' -type 'f')
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

ASMSRCS = $(shell find $(MODULES) -name '*.S' -type 'f')
ASMOBJS = $(ASMSRCS:.S=.o)

LDS = kernel.lds

all: kernel.elf

kernel.elf: $(LDS) $(OBJS) $(ASMOBJS)
	$(LD) -T$(LDS) $(LDFLAGS) -o $@ $(OBJS) $(ASMOBJS)

$(ASMOBJS): $(ASMSRCS)
	$(AS) $(ASMFLAGS) -o $@ -f elf64 $(@:.o=.S)

%.d: %.c
	rm -f $@; \
	$(CC) $(CFLAGS) -MM -MT "$@ $(@:.d=.o)" $< -o $@ \

PHONY += clean
clean:
	rm -f kernel.elf $(OBJS) $(ASMOBJS) $(DEPS)

include $(DEPS)

.PHONY: $(PHONY)
