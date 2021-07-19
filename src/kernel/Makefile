LIBDIR := -L$(ROOTDIR)/lib/

#LIBS := -lgnuefi
LIBS := -lefi

INCLUDEFLAGS := -I$(ROOTDIR)/lib/include/
INCLUDEFLAGS += -I$(ROOTDIR)/include/

LINKER := x86_64-w64-mingw32-gcc
LFLAGS := -nostdlib -Wl,-dll -shared -Wl,--subsystem,33 $(LIBDIR) $(LIBS) -e KeMain

CC := x86_64-w64-mingw32-gcc
CFLAGS := $(INCLUDEFLAGS) -m64 -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

CFILES = $(shell find $(CWD) -name '*.c')
OBJS = $(CFILES:.c=.o)
DEPS = $(shell find $(CWD:src=include) -name '*.h')

%.o: %.c $(DEPS)
	@echo "[+] Building $<"
	$(CC) -c -o $@ $< $(CFLAGS)

kernel.exe: $(OBJS)
	@echo "[+] Building kernel.exe"
	echo $(OBJS)
	$(LINKER) $(LFLAGS) -o $(ROOTDIR)/$@ $^
