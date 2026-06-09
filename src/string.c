#include <stddef.h>
#include <stdint.h>

static char strtemp[32];

size_t strlen(const char* str) {
	size_t size = 0;
	while(*str != '\0') {
		size++;
		str++;
	}

	return size;
}

const char* struint(unsigned int a) {
	char* out = &(strtemp[sizeof(strtemp) - 1]);
	strtemp[sizeof(strtemp) - 1] = 0;
	
	do {
		out--;
		(*out) = '0' + (a % 10);
		a /= 10;
	} while (a > 0);

	return out;
}

const char* struint_hex(unsigned int a, int length) {
	char* out = &(strtemp[sizeof(strtemp) - 1]);
	strtemp[sizeof(strtemp) - 1] = 0;
	
	for (int i = 0; i < length; i++) {
		register int b = a % 16;
		out--;
		if (b < 10) {
			(*out) = '0' + b;
		} else {
			(*out) = 'a' + b - 10;
		}
		a /= 16;
	}

	return out;
}

const char* struint_bin(unsigned int a, int length) {
	char* out = &(strtemp[sizeof(strtemp) - 1]);
	strtemp[sizeof(strtemp) - 1] = 0;
	
	for (int i = 0; i < length; i++) {
		out--;
		(*out) = '0' + (a % 2);
		a /= 2;
	}

	return out;
}
