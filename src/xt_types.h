#ifndef XT_TYPES_H
#define XT_TYPES_H

#include <stdint.h>
#define PATTERN_LEN 64

// Arrangement table dictating order and channel numbers for patterns
/*typedef struct PatternTable
{
	
} PatternTable;*/

// A single line of a phrase pattern
typedef struct PatternCell
{
	uint8_t note;
	uint8_t inst;
	uint8_t cmd1;
	uint8_t arg1;
	uint8_t cmd2;
	uint8_t arg2;
} PatternCell;

// Note values:
#define NOTE_NONE      0x00
#define NOTE_CUT       0xFF
#define NOTE_OFF       0xFE
#define NOTE_VALID     0x01

// Effects:
#define EFFECT_NONE    '\0'
#define EFFECT_VIBRATO 'V'
#define EFFECT_PAN     'O'
#define EFFECT_SLIDE   'S'
#define EFFECT_DELAY   'D'
#define EFFECT_CUT     'K'
#define EFFECT_HOP     'H'
#define EFFECT_BREAK   'B'
#define EFFECT_RATE    'R'
#define EFFECT_VOLUME  'A'
#define EFFECT_TEMPO   'T'
#define EFFECT_ALGO    'A'
#define EFFECT_OP1_TL  '1'
#define EFFECT_OP2_TL  '2'
#define EFFECT_OP3_TL  '3'
#define EFFECT_OP4_TL  '4'
#define EFFECT_FB      'F'


#define NOTE_A  0x00
#define NOTE_AS 0x01
#define NOTE_B  0x02
#define NOTE_C  0x03
#define NOTE_CS 0x04
#define NOTE_D  0x05
#define NOTE_DS 0x06
#define NOTE_E  0x07
#define NOTE_F  0x08
#define NOTE_FS 0x09
#define NOTE_G  0x0A
#define NOTE_GS 0x0B
// 0x01 and on are
// Octave = (note value - 1) / 12
// Semitone = (note value - 1) % 12


#endif
