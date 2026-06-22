#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define io_wait() outb(0x80, 0)

/* Master and slave (1 & 2) pics command and data ports */
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_ICW4 0x01 /* Indicates that ICW4 will be present */
#define ICW1_INIT 0x10 /* Initialization - required! */

#define ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */

#define CASCADE_IRQ 2

#define PIC_EOI 0x20

/* Low level IO */
uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);
uint16_t inw(uint16_t);
void outw(uint16_t, uint16_t);

void pic_set_offsets(uint8_t pic1_offset, uint8_t pic2_offset) {
	/* Init in cascade mode */
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	/* ICW2: Master PIC vector offset */
	outb(PIC1_DATA, pic1_offset);
	io_wait();
	/* ICW2: Slave PIC vector offset */
	outb(PIC2_DATA, pic2_offset);
	io_wait();
	/* ICW3: tell Master PIC that there is a slave PIC at IRQ2 */
	outb(PIC1_DATA, 1 << CASCADE_IRQ);
	io_wait();
	/* ICW3: tell Slave PIC its cascade identity (0000 0010) */
	outb(PIC2_DATA, 2);
	io_wait();
	
	/* ICW4: have the PICs use 8086 mode (and not 8080 mode) */
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
}

void pic_set_masks(uint8_t pic1_mask, uint8_t pic2_mask) {
	outb(PIC1_DATA, pic1_mask);
	io_wait();
	outb(PIC2_DATA, pic2_mask);
	io_wait();
}

void pic_low_eoi() {
	outb(PIC1_COMMAND, PIC_EOI);
}

void pic_high_eoi() {
	outb(PIC2_COMMAND, PIC_EOI);
}
