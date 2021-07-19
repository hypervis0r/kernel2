OVMF = /usr/share/ovmf/OVMF.fd

export ROOTDIR = $(shell pwd)

.PHONY: all clean run

# UEFI Bootloader
boot.efi:
	make $@ -C ./src/boot

kernel.exe:
	make $@ -C ./src/kernel

kernel2.img: boot.efi kernel.exe
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT/BOOTX64.efi
	mcopy -i $@ kernel.exe ::/KERNEL.EXE
	@echo "[+] Built kernel2"

all: clean kernel2.img
	@echo "[+] Build completed"

clean:
	rm -rf boot.efi kernel.exe kernel2.img
	find . -type f -name '*.o' -delete

run: all
	qemu-system-x86_64 -serial stdio -cpu qemu64 -bios $(OVMF) -s -net none kernel2.img
