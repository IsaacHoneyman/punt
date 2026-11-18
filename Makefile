CXX := clang++
LD  := ld.lld

CXXFLAGS := --target=x86_64-elf -std=c++23 -O2 -g -Wall -Wextra \
            -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector \
            -fno-threadsafe-statics -fno-pic -mno-red-zone \
            -mgeneral-regs-only -mcmodel=kernel -Isrc
LDFLAGS  := -static -z max-page-size=0x1000 -T linker.ld

SRCS   := $(wildcard src/*.cpp)
OBJS   := $(SRCS:src/%.cpp=build/%.o)
KERNEL := build/kernel
ISO    := build/punt.iso

QEMU := qemu-system-x86_64 -M q35 -m 256M -cdrom $(ISO) -serial stdio -no-reboot

.PHONY: all run debug clean

all: $(ISO)

# Compile each .cpp to a .o (plus a .d file listing the headers it uses)
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Link all objects into the kernel ELF
$(KERNEL): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# Lay out the ISO contents, pack them, and make it BIOS-bootable
$(ISO): $(KERNEL) limine.conf
	rm -rf iso_root
	mkdir -p iso_root/boot/limine iso_root/EFI/BOOT
	cp $(KERNEL) iso_root/boot/kernel
	cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
	   limine/limine-uefi-cd.bin iso_root/boot/limine/
	cp limine/BOOTX64.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
	    -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
	    -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
	    -efi-boot-part --efi-boot-image --protective-msdos-label \
	    iso_root -o $@

run: $(ISO)
	$(QEMU)

# Log every interrupt/exception and wait for GDB on port 1234
debug: $(ISO)
	$(QEMU) -d int -s -S

clean:
	rm -rf build iso_root

-include $(OBJS:.o=.d)