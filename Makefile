OVMF = /usr/share/ovmf/OVMF.fd

export ROOTDIR = $(shell pwd)

.PHONY: all clean run

# UEFI Bootloader
boot.efi:
	@echo "[+] Building kernel2.efi"
	make $@ -C ./src/boot

kernel2.img: boot.efi
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT/BOOTX64.efi	
	@echo "[+] Built kernel2"

all: clean kernel2.img
	@echo "[+] Build completed"

clean:
	rm -rf boot.efi kernel2.img
	find . -type f -name '*.o' -delete

run: all
	qemu-system-x86_64 -serial stdio -cpu qemu64 -bios $(OVMF) -net none kernel2.img
