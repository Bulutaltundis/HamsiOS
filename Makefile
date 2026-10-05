AS=nasm
CC=gcc
LD=ld

CFLAGS=-m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs -Wall -Wextra

LDFLAGS=-m elf_i386 -T linker.ld


all: hamsios.iso


boot.o: boot/boot.asm
	$(AS) -f elf32 boot/boot.asm -o boot.o


gdt.o: kernel/gdt.asm
	$(AS) -f elf32 kernel/gdt.asm -o gdt.o


gdt_c.o: kernel/gdt.c
	$(CC) $(CFLAGS) -c kernel/gdt.c -o gdt_c.o


interrupts.o: kernel/interrupts.asm
	$(AS) -f elf32 kernel/interrupts.asm -o interrupts.o


kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o


idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c kernel/idt.c -o idt.o


pic.o: kernel/pic.c
	$(CC) $(CFLAGS) -c kernel/pic.c -o pic.o


keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o keyboard.o


shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c kernel/shell.c -o shell.o


hamsios.bin: \
	boot.o \
	gdt.o \
	gdt_c.o \
	interrupts.o \
	kernel.o \
	idt.o \
	pic.o \
	keyboard.o \
	shell.o \
	linker.ld

	$(LD) $(LDFLAGS) \
		boot.o \
		gdt.o \
		gdt_c.o \
		interrupts.o \
		kernel.o \
		idt.o \
		pic.o \
		keyboard.o \
		shell.o \
		-o hamsios.bin


iso/boot/hamsios.bin: hamsios.bin
	cp hamsios.bin iso/boot/hamsios.bin


hamsios.iso: iso/boot/hamsios.bin iso/boot/grub/grub.cfg
	grub-mkrescue -o hamsios.iso iso


run: hamsios.iso
	qemu-system-i386 -cdrom hamsios.iso


clean:
	rm -f boot.o
	rm -f gdt.o
	rm -f gdt_c.o
	rm -f interrupts.o
	rm -f kernel.o
	rm -f idt.o
	rm -f pic.o
	rm -f keyboard.o
	rm -f shell.o

	rm -f hamsios.bin
	rm -f hamsios.iso

	rm -f iso/boot/hamsios.bin
