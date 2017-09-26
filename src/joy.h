#ifndef JOY_H
#define JOY_H

#include <stdint.h>

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 4
#define KEY_RIGHT 8
#define KEY_A 16
#define KEY_B 32

#define JOY1_ADDR 0xE9A001

static inline uint8_t joy_read(uint8_t id)
{
	volatile uint8_t *joypad = (volatile uint8_t *)JOY1_ADDR;
	joypad += (id ? 2 : 0);
	return *joypad;
}

#endif // JOY_H
