GNUEFI := ./gnuefi

LIBDIR := -L./lib/
LIBDIR += -L$(GNUEFI)

LIBS := -lgnuefi
LIBS += -lefi

INCLUDEFLAGS := -Ilib/include/
INCLUDEFLAGS += -Iinclude/

LINKER := x86_64-w64-mingw32-gcc
LFLAGS := -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 $(LIBDIR) $(LIBS) -e efi_main 

CC := x86_64-w64-mingw32-gcc
CFLAGS := $(INCLUDEFLAGS) -m64 -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

CFILES = $(shell find ./src -name '*.c')
OBJS = $(CFILES:.c=.o)
DEPS = $(shell find ./include -name '*.h')

OVMF = /usr/share/ovmf/OVMF.fd

.PHONY: all clean run

%.o: %.c $(DEPS)
	@echo "[+] Building $<"
	$(CC) -c -o $@ $< $(CFLAGS)

kernel2.efi: $(OBJS)
	@echo "[+] Building kernel2.efi"
	$(LINKER) $(LFLAGS) -o $@ $^

kernel2.img: kernel2.efi
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT/BOOTX64.efi	
	@echo "[+] Built kernel2"

all: clean kernel2.img
	@echo "[+] Build completed"

clean:
	rm -rf $(OBJS) kernel2.efi kernel2.img

run: all
	qemu-system-x86_64 -serial stdio -cpu qemu64 -bios $(OVMF) -net none kernel2.img
