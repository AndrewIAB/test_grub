.set F_ALIGN,    1<<0                 /* align loaded modules on page boundaries */
.set F_MEMINFO,  1<<1                 /* provide memory map */
.set F_VIDEO,    1<<2                 /* provide video mode table */
.set FLAGS,      F_ALIGN | F_MEMINFO | F_VIDEO
.set MAGIC,      0x1BADB002           /* 'magic number' lets bootloader find the header */
.set CHECKSUM,   -(MAGIC + FLAGS)     /* checksum of above, to prove we are multiboot */

// Multiboot signiture for GRUB
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.skip 24
// Graphics mode: 0 for linear, 1 for text
.long 0
// Width
.long 320
// Height
.long 200
// Depth
.long 32

// Stack
.section .bss
// Stack must be 16-byte aligned
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

// Present. Always 1
.set GDTA_P,   0b10000000
// Is user-space
.set GDTA_R3,  0b01100000
// Descriptor type. Always 1 for code/data
.set GDTA_S,   0b00010000
// Executable
.set GDTA_E,   0b00001000
// Enables read access for code segments or write access for data segments. Always 1
.set GDTA_RW,  0b00000010
// Accessed. Always 1
.set GDTA_A,   0b00000001

// Base with required flags set and unused unset
// Describes a kenel data-segment
.set GDTA_KDBASE, GDTA_P | GDTA_S | GDTA_RW | GDTA_A

// Flags of all segments
// 32-bit mode segments with page granularity (to cover full address space)
.set GDTFLAGS, 0b1100

.section .data
// Align to page boundary (so I can relocate it when switching to paging)
.align 4096

_gdt_descriptor:
.short _gdt_end - _gdt_start
.int _gdt_start
// Flat memory model
_gdt_start:
// Null
.quad 0x00000000

// Ring 0 data
.short 0xFFFF
.short 0x0000
.byte 0x00
// Access byte
.byte GDTA_KDBASE
// Limit 16-19 (0-3) | Flags (4-7)
.byte 0xF | (GDTFLAGS << 4)
.byte 0x00

// Ring 0 code
.short 0xFFFF
.short 0x0000
.byte 0x00
// Access byte
.byte GDTA_KDBASE | GDTA_E
// Limit 16-19 (0-3) | Flags (4-7)
.byte 0xF | (GDTFLAGS << 4)
.byte 0x00

// Ring 3 data
.short 0xFFFF
.short 0x0000
.byte 0x00
// Access byte
.byte GDTA_KDBASE | GDTA_R3
// Limit 16-19 (0-3) | Flags (4-7)
.byte 0xF | (GDTFLAGS << 4)
.byte 0x00

// Ring 3 code
.short 0xFFFF
.short 0x0000
.byte 0x00
// Access byte
.byte GDTA_KDBASE | GDTA_E | GDTA_R3
// Limit 16-19 (0-3) | Flags (4-7)
.byte 0xF | (GDTFLAGS << 4)
.byte 0x00

_gdt_end:

// Move to lowest point in memory
.set _gdt_descriptor_addr, 0

.set SEG_R0D, 8 * 1
.set SEG_R0C, 8 * 2
.set SEG_R3D, 8 * 3
.set SEG_R3C, 8 * 4

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp

// Push as arguments to kernel_main function
	push %eax
	push %ebx

// Copy GDT to address _gdt_descriptor_addr in lowest memory
	mov $_gdt_descriptor, %ESI
	mov $_gdt_descriptor_addr, %EDI
	mov $(_gdt_end-_gdt_descriptor), %ECX
	rep movsl

	lgdt _gdt_descriptor_addr

	mov $SEG_R0D, %AX
	mov %AX, %DS
	mov %AX, %ES
	mov %AX, %FS
	mov %AX, %GS
	mov %AX, %SS
	ljmp $SEG_R0C, $_long_jump

_long_jump:
	
	call kernel_main

	cli
	hlt
// 1b means first occurance of the 1: label searching backwards
1:	jmp 1b

.size _start, . - _start
