export PATH := $(PATH):/home/andrew/opt/cross/bin/
CC:=i686-elf-gcc
AS:=i686-elf-as

CARGS:=-ffreestanding -nostdlib -fno-pic -std=gnu99 -Wall -Wextra -Ofast -g1

C_FILES:=$(wildcard src/*.c) $(wildcard src/**/*.c)
C_NAMES:=$(patsubst src/%, %, $(basename $(C_FILES)))
OBJ_FILES:=$(addprefix bin/obj/, $(addsuffix .o, $(C_NAMES)))

DISK_SIZE := $(shell echo $$((512 * 1024 * 1024)))
DD_BS := 4096
DD_COUNT := $(shell expr $(DISK_SIZE) / $(DD_BS))

BIN:=bin/kernel

.PHONY: kernel
kernel: $(BIN)

.PHONY: all
all: disk

.PHONY: disk
disk: disk.img

.PHONY: clean
clean:
	rm bin/* -rf
	rm disk.img -f
	
	find src -type d -exec mkdir -p "$(shell pwd)/bin/{}" \;
	mv bin/src bin/obj

.PHONY: test
test:
	qemu-system-i386 -d int -drive file=disk.img,media=disk,format=raw -m 512M -device isa-vga

.PHONY: run
run: all test

.PHONY: tell
tell:
	@echo $(C_FILES)
	@echo $(C_NAMES)
	@echo $(OBJ_FILES)
	@pwd

disk.img: $(BIN) grub.cfg
	rm diskloop -f
	rm disk.img -f
# Create mounting point
	mkdir mnt -p
# Generate zerod out binary
	dd if=/dev/zero of=$@ bs=$(DD_BS) count=$(DD_COUNT)
# Loop setup, create loop device from binary. Store used loop device in file diskloop.
	sudo losetup -f --show $@ > diskloop
# Feed script from layout.txt into ifstream of sfdisk (format drive)
	cat layout.txt | sudo sfdisk $$(cat diskloop)
# Probe for created partition
	sudo partprobe $$(cat diskloop)
# Create filesystem on partition
	sudo mkfs.ext2 $$(cat diskloop)p1
# Mount partition and install grub
	sudo mount $$(cat diskloop)p1 mnt
	sudo mkdir mnt/boot/grub -p
	sudo grub-install --target=i386-pc --boot-directory=mnt/boot $$(cat diskloop)
# Copy files into filesystem
	sudo cp grub.cfg mnt/boot/grub
	sudo cp $(BIN) mnt/boot
# Cleanup
	sudo umount mnt
	sudo losetup -d $$(cat diskloop)
	rm mnt -r

$(BIN): bin/boot.so $(OBJ_FILES)
	$(CC) -T linker.ld -o $@ $^ $(CARGS) -lgcc

bin/boot.so: boot.s
	$(AS) $< -o $@

bin/obj/%%/%.o: src/%%/%.c
	$(CC) -c $< -o $@ $(CARGS)

bin/obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CARGS)
