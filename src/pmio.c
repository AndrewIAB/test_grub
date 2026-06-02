#include <stdint.h>

void outb(uint16_t address, uint8_t data) {
	__asm__ volatile(
			 "outb %b0, %w1"
			:
			:"a"(data), "Nd"(address)
			:"memory");
}

uint8_t inb(uint16_t address) {
	uint8_t data;
	__asm__ volatile(
			 "inb %w1, %b0"
			:"=a"(data)
			:"Nd"(address)
			:"memory");
	return data;
}

void outw(uint16_t address, uint16_t data) {
	__asm__ volatile(
			 "outw %w0, %w1"
			:
			:"a"(data), "Nd"(address)
			:"memory");
}

uint16_t inw(uint16_t address) {
	uint16_t data;
	__asm__ volatile(
			 "inw %w1, %w0"
			:"=a"(data)
			:"Nd"(address)
			:"memory");
	return data;
}
