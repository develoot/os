SYSLIB = /usr/lib
EFIINC = /usr/include/efi
CFLAGS = -I$(SRC) -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol \
		 -Wall -Wextra -fpie -ffreestanding -fshort-wchar -g -DDEBUG_ASSERT
LDFLAGS = -static -Bsymbolic -nostdlib

SRCS = $(shell find . -name '*.c' -type 'f') $(shell find $(SRC) -name '*.c' -type 'f')
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

LDS = kernel.lds

all: kernel.elf

kernel.elf: $(LDS) $(OBJS)
	$(LD) -T$(LDS) $(LDFLAGS) -o $@ $(OBJS)

PHONY += clean
clean:
	rm -f kernel.elf $(OBJS) $(DEPS)

%.d: %.c
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: $(PHONY)

include $(DEPS)
