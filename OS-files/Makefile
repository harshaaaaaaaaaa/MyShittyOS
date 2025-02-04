GPPPARAMS = -m32 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -std=c++11
ASPARAMS =  --32 --gstabs
LDPARAMS = -m elf_i386 -T linker.ld --no-warn-rwx-segments

objects = loader.o gdt.o port.o interruptstubs.o interrupts.o kernel.o

%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

shit.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/mykernel.bin
	echo 'set timeout=5s' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "ShitOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

.PHONEY: clean 
clean: 
		rm -f $(objects) mykernel.bin shit.iso


