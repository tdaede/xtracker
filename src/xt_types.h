#ifndef XT_TYPES_H
#define XT_TYPES_H

#include <stdint.h>

#define XT_SONG_TITLE_LENGTH 64

#define XT_PHRASE_MAX_ROWS 64
#define XT_PHRASES_PER_CHANNEL 64
#define XT_FM_CHANNEL_COUNT 8
#define XT_PCM_CHANNEL_COUNT 4
#define XT_TOTAL_CHANNEL_COUNT (XT_FM_CHANNEL_COUNT + XT_PCM_CHANNEL_COUNT)
#define XT_FRAME_COUNT 128

// XtCmd tries to keep parity with Famitracker's effects.
typedef enum XtCmd
{
	XT_CMD_NONE = '\0',
	XT_SLIDE_UP = '1',
	XT_SLIDE_DOWN = '2',
	XT_PORTAMENTO = '3',
	XT_CMD_VIBRATO = '4',
	XT_CMD_TREMOLO = '7',

	XT_CMD_BREAK = 'B',
	XT_CMD_HALT = 'C',
	XT_CMD_SKIP = 'D',

	XT_CMD_SPEED = 'F',
	XT_CMD_NOTE_DELAY = 'G',

	XT_CMD_TL_OP0 = 'H',
	XT_CMD_TL_OP1 = 'I',
	XT_CMD_TL_OP2 = 'J',
	XT_CMD_TL_OP3 = 'K',

	XT_CMD_NOISE_EN = 'N',

	XT_CMD_TUNE = 'P',

	XT_CMD_SLIDE_UP = 'Q',
	XT_CMD_SLIDE_DOWN = 'R',
	XT_CMD_MUTE_DELAY = 'S',

	XT_CMD_MULT_OP0 = 'V',
	XT_CMD_MULT_OP1 = 'X',
	XT_CMD_MULT_OP2 = 'Y',
	XT_CMD_MULT_OP3 = 'Z',

	XT_CMD_MAX = 0xFF;
} XtCmd;

typedef enum XtNote
{
	XT_NOTE_NONE = 0x00,
	// Main notes.
	XT_NOTE_A = 0x01,
	XT_NOTE_AS = 0x02,
	XT_NOTE_B = 0x03,
	XT_NOTE_C = 0x04,
	XT_NOTE_CS = 0x05,
	XT_NOTE_D = 0x06,
	XT_NOTE_DS = 0x07,
	XT_NOTE_E = 0x08,
	XT_NOTE_F = 0x09,
	XT_NOTE_FS = 0x0A,
	XT_NOTE_G = 0x0B,
	XT_NOTE_GS = 0x0C,
	XT_NOTE_OFF = 0x0D,
	XT_NOTE_CUT = 0x0E,

	// The octave is encoded in three bits (0 - 7).
	XT_NOTE_OCTAVE_MASK = 0x70,

	// If the highest bit is set, no key-on will be sent.
	XT_NOTE_NO_KEY_ON_MASK = 0x80,

	XT_NOTE_MAX = 0xFF,
} XtNote;

// A single line of a phrase pattern.
typedef struct XtCell
{
	XtNote note;
	uint8_t inst;
	XtCmd cmd1;
	uint8_t arg1;
	XtCmd cmd2;
	uint8_t arg2;
} XtCell;

// Vertical collection of cells for one channel.
typedef struct XtPhrase
{
	XtCell cells[XT_PHRASE_MAX_ROWS];
	uint16_t phrase_valid;
} XtPhrase;

// One row within the arrangement table.
typedef struct XtFrame
{
	XtPhrase *row[XT_TOTAL_CHANNEL_COUNT];
	uint16_t row_idx[XT_TOTAL_CHANNEL_COUNT];
} XtFrame;

typedef struct XtInstrument
{
	uint8_t reg_20_pan_fl_con;  // Rch (1), Lch(1), FL(3), CON(3)
	uint8_t reg_28_oct_note,;  // null (1), Oct(3), Note(4)
	uint8_t reg_30_kf;  // KF (6), null (2)
	uint8_t reg_38_pms_ams;  // null (1), PMS(3), null(2), AMS(2)
	uint8_t reg_40_det_mul[4]:  // null (1), DT1(3), MUL(4)
	uint8_t reg_60_tl_op1[4]:  // null(1), TL(7)
	uint8_t reg_80_ks_ar[4]:  // ks(2), null(1), AR(5)
	uint8_t reg_A0_ams_d1r[4]:  // ame(1), null(2), D1R(5)
	uint8_t reg_C0_dt2_d2r[4]:  // dt2(2), null(1), D2R(5)
	uint8_t reg_E0_d1l_rr[4]:  // D1L(4), RR(4)
	uint8_t reg_

} XtInstrument;

// One whole song in memory.
typedef struct XtSong
{
	XtPhrase phrases[XT_TOTAL_CHANNEL_COUNT * XT_PHRASES_PER_CHANNEL];
	XtFrame frames[XT_FRAME_COUNT];
	char title[XT_SONG_TITLE_LENGTH];

	int16_t speed;  // Number of ticks that elapse per row (less = faster).
	int16_t tempo;  // Frequency of ticks (TODO: Add a unit, and hook this up).

	int16_t phrase_length;  // How many cell rows are in a phrase.
	int16_t frame_count;  // Length of the song in arrangement rows.

	int16_t loop_point;  // Arrangement row to return to (-1 for no repeat).
} XtSong;

#endif
