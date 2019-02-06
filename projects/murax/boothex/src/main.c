#include <stdint.h>

#include <murax.h>

#define NULL 0

#define PROMPT 0
#define NEXT 1
#define LOOP 2

static int c, cnt, pos, val, len;
static char *cp;
static void *base_addr = NULL;
static int state;

void putch(char c) {
	UART->DATA = c;
}

void print(const char *p) {
	while (*p) putch(*(p++));
}

void prompt() {
	print("\r\nrv32> ");
}

void main() {
        // set 4 output bits for LEDs
	GPIO_A->OUTPUT_ENABLE = 0x0000000F;
	GPIO_A->OUTPUT = 0x00000000;

	GPIO_A->OUTPUT |= 0x01; // Set Red LED

        state = PROMPT;

	// Loop reading characters from UART
        while (1) {
		if (UART->STATUS >> 24){         // UART RX interrupt?
			GPIO_A->OUTPUT ^= 0x02;  // Toggle Yellow Led

                        // Output prompt
			if (state == PROMPT) { 
				prompt();
				state = NEXT;
			}

                        // Prepare for next SREC
			if (state == NEXT) {
				pos = -1;
				len = 255;
				state = LOOP;
			}

			c = (UART->DATA) & 0xFF; // Get character
			putch(c);                // Echo character

			// Look for SREC
			if (pos < 0) {
				if (c == 'S') {
					pos = 0;
				} else if (c == '\r') {
					state = PROMPT;
					continue;
				}
				val = 0;
				continue;
			} 

			// End of SREC
			if (c >= 10 && c <= 13) {
				state = NEXT;
				continue;
			}

			// Convert to hex
			val <<= 4;

			if (c >= 'a') c -= 32;

			if (c >= 'A') val |= c - 'A' + 10;
			else val |= c - '0';

			// Move along SREC
			pos++;

			// Check SREC type
			if (pos == 1) {
				if (val >= 7 && val <= 9) { // Start address record
					__asm __volatile__(
					"li s0, 0x80002000;"	/* 8K RAM top = stack address */
					"mv ra, zero;"
					"jr %0;"
					: 
					: "r" (base_addr)
					);
				}
				if (val <= 3) len = (val << 1) + 5; // Get length of record
				val = 0;
				state = LOOP;
				continue;
			}

			// Byte count
			if (pos == 3) {
				cnt = 2 + (val << 1);
				val = 0;
				continue;
			}
			 
			// Valid length? Skips S0 record
			if (len < 6) continue;
			
			// End of address
			if (pos == len) {
				cp = (char *) val;
				// Use first record as Base address
				if (base_addr == NULL) base_addr = (void *) val;
				continue;
			}

			// Write byte to RAM
			if (pos > len && (pos & 1) && pos < cnt) *cp++ = val;
		}
	}
}

void irqCallback(){
}
