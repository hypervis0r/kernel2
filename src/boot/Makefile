LIBDIR := -L$(ROOTDIR)/lib/

#LIBS := -lgnuefi
LIBS := -lefi

INCLUDEFLAGS := -I$(ROOTDIR)/lib/include/
INCLUDEFLAGS += -I$(ROOTDIR)/include/

LINKER := x86_64-w64-mingw32-gcc
LFLAGS := -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 $(LIBDIR) $(LIBS) -e efi_main 

CC := x86_64-w64-mingw32-gcc
CFLAGS := $(INCLUDEFLAGS) -m64 -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

CFILES = $(shell find $(CWD) -name '*.c')
OBJS = $(CFILES:.c=.o)
DEPS = $(shell find $(CWD:src=include) -name '*.h')

%.o: %.c $(DEPS)
	@echo "[+] Building $<"
	$(CC) -c -o $@ $< $(CFLAGS)

boot.efi: $(OBJS)
	@echo "[+] Building boot.efi"
	echo $(OBJS)
	$(LINKER) $(LFLAGS) -o $(ROOTDIR)/$@ $^

