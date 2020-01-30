#include "xt_render.h"

#include "common.h"
#include <stdio.h>

#include "x68000/x68k_pcg.h"

#define PATTERN_DRAW_MAX_LENGTH 48

#define PAL_NOTE    0
#define PAL_INST    0
#define PAL_CMD     0
#define PAL_ARG     0

	// These strings look odd because they align to tiles in letter positions
	// for non-letter things (e.g. '-'. '#').
	// h = '#'
	// i = '='
	// j = '^'
	// k = '-' (left)
	// l = '-' (right)

// Labels starting at XT_NOTE_NONE.
static const uint8_t note_labels[] =
{
	"kl"
	"ch"
	"dl"
	"dh"
	"el"
	"fl"
	"fh"
	"gl"
	"gh"
	"al"
	"ah"
	"bl"
	"cl"
};

void xt_draw_phrase(XtPhrase *phrase, int16_t x)
{
	const int cell_width_cells = 7;
	const int nt_width_cells = 512 / 8;
	int num_cells = ARRAYSIZE(phrase->cells) - 1;
	const XtCell *cell = &phrase->cells[0];
	volatile uint16_t *nt0 = (volatile uint16_t *)PCG_BG0_NAME;
	volatile uint16_t *nt1 = (volatile uint16_t *)PCG_BG1_NAME;
	nt0 += x;
	nt1 += x;
	do
	{
		// Note column. Notes get full-width chars. They are a little narrower than
		// the alotted 7px, so letter characters are pushed 1px to the right to let
		// the whole column have a margin. Characters on the right side ('-', '#',
		// etc) are pushed one pixel to the left correspondingly.
		// TODO: Change the backing using BG1 for no-key-on notes.
		*nt1++ = 0;
		*nt1++ = 0;
		if (cell->note == XT_NOTE_OFF)
		{
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, 'i');
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, 'i');
		}
		else if (cell->note == XT_NOTE_CUT)
		{
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, 'j');
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, 'j');
		}
		else
		{
			const uint8_t note = cell->note & XT_NOTE_TONE_MASK;
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, note_labels[note * 2]);
			*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, note_labels[1 + note * 2]);
		}

		const uint8_t octave = cell->note >> 4;
		*nt0++ = PCG_ATTR(0, 0, PAL_NOTE, 0x01 + octave);

		// Instrument.
		const uint8_t instr_high = 0x70 + (cell->inst >> 4);
		const uint8_t instr_low = 0x70 + (cell->inst & 0x0F);
		*nt1++ = PCG_ATTR(0, 0, PAL_INST, instr_high);
		*nt0++ = PCG_ATTR(0, 0, PAL_INST, instr_low);
		nt1++;

		// Command 1.
		const uint16_t empty_cmd_column = PCG_ATTR(0, 0, PAL_CMD, '_');
		const uint16_t empty_arg_column = PCG_ATTR(0, 0, PAL_ARG, '_');
		if (cell->cmd1 == XT_CMD_NONE)
		{
			*nt0++ = empty_cmd_column;
			*nt1++ = empty_arg_column;
			*nt0++ = empty_arg_column;
		}
		else
		{
			const uint8_t arg_high = 0x10 + (cell->arg1 >> 4);
			const uint8_t arg_low = 0x10 + (cell->arg1 & 0xF);
			*nt0++ = PCG_ATTR(0, 0, PAL_CMD, cell->cmd1);
			*nt1++ = PCG_ATTR(0, 0, PAL_ARG, arg_high);
			*nt0++ = PCG_ATTR(0, 0, PAL_ARG, arg_low);
		}
		// Command 2.
		if (cell->cmd2 == XT_CMD_NONE)
		{
			*nt1++ = empty_cmd_column;
			*nt0++ = empty_arg_column;
			*nt1++ = empty_arg_column;
		}
		else
		{
			const uint8_t arg_high = 0x10 + (cell->arg2 >> 4);
			const uint8_t arg_low = 0x10 + (cell->arg2 & 0xF);
			*nt1++ = PCG_ATTR(0, 0, PAL_CMD, cell->cmd2);
			*nt0++ = PCG_ATTR(0, 0, PAL_ARG, arg_high);
			*nt1++ = PCG_ATTR(0, 0, PAL_ARG, arg_low);
		}
		cell++;
		nt0 += (nt_width_cells - cell_width_cells);
		nt1 += (nt_width_cells - cell_width_cells);
	} while (num_cells--);
}
