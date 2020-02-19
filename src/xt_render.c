#include "xt_render.h"

#include "common.h"
#include <string.h>

#include "x68000/x68k_pcg.h"
#include "x68000/x68k_vidcon.h"

static const uint16_t default_palette[] =
{
	// Line 1 - Notes/Octaves, Instruments, Commands
	PAL_RGB8(0x00, 0x00, 0x00),
	PAL_RGB8(0xFF, 0xFF, 0xFF),  // Note letters
	PAL_RGB8(0x40, 0x40, 0x40),  // Note dash
	PAL_RGB8(0x5D, 0xFF, 0x5D),  // Octave (top number row)
	PAL_RGB8(0x33, 0x66, 0x00),  // Octave dash (top number row)
	PAL_RGB8(0xFF, 0xCC, 0x22),  // Instrument (7th number row)
	PAL_RGB8(0x99, 0x33, 0x0B),  // Instrument dash (top row 0x0A)
	PAL_RGB8(0xFF, 0x33, 0x99),  // Command (normal alpha)
	PAL_RGB8(0x99, 0x08, 0x99),  // Command dash (top row 0x0B)
	PAL_RGB8(0x66, 0x66, 0xFF),  // Arg (row 1(
	PAL_RGB8(0x08, 0x33, 0x99),  // Arg dash (top row 0x0C)
	PAL_RGB8(0x33, 0x00, 0x99),  // Separator
	PAL_RGB8(0x00, 0x00, 0x00),
	PAL_RGB8(0x00, 0x00, 0x00),
	PAL_RGB8(0x10, 0x10, 0xFF),  // Cursor
	PAL_RGB8(0x00, 0x00, 0x00),  // Background
};

// These strings look odd because they align to tiles in letter positions
// for non-letter things (e.g. '-'. '#').
// h = '#'
// i = '=' (left)
// j = '^' (left)
// m = '^' (right)
// k = '-' (left)
// l = '-' (right)
// n = '=' (right)

// Labels starting at XT_NOTE_NONE.
static const uint8_t note_labels[] =
{
	"kl"  // 0x00 no note
	"ch"  // 0x01 CS
	"dl"  // 0x02 D
	"dh"  // 0x03 DS
	"XX"  // 0x04 
	"el"  // 0x05 E
	"fl"  // 0x06 F
	"fh"  // 0x07 FS
	"XX"  // 0x08
	"gl"  // 0x09 G
	"gh"  // 0x0A GS
	"al"  // 0x0B A
	"XX"  // 0x0C
	"ah"  // 0x0D AS
	"bl"  // 0x0E B
	"cl"  // 0x0F C
};

static inline void xt_draw_fm_phrase(XtPhrase *phrase, uint16_t x,
                                     uint16_t height)
{
	const int cell_width_cells = 7;
	const int nt_width_cells = 512 / 8;
	const XtCell *cell = &phrase->cells[0];
	volatile uint16_t *nt0 = (volatile uint16_t *)PCG_BG0_NAME;
	volatile uint16_t *nt1 = (volatile uint16_t *)PCG_BG1_NAME;
	nt0 += x;
	nt1 += x;
	for (uint16_t i = 0; i < height; i++)
	{
		const uint8_t pal = 1;
		// Note column. Notes get full-width chars. They are a little narrower than
		// the alotted 7px, so letter characters are pushed 1px to the right to let
		// the whole column have a margin. Characters on the right side ('-', '#',
		// etc) are pushed one pixel to the left correspondingly.
		// TODO: Change the backing using BG1 for no-key-on notes.
		*nt1++ = PCG_ATTR(0, 0, pal, 0x00);
		*nt1++ = PCG_ATTR(0, 0, pal, 0x00);
		if (cell->note == XT_NOTE_OFF)
		{
			*nt0++ = PCG_ATTR(0, 0, pal, 'i');
			*nt0++ = PCG_ATTR(0, 0, pal, 'n');

			*nt0++ = PCG_ATTR(0, 0, pal, 9);

			*nt1++ = PCG_ATTR(0, 0, pal, 10);
			*nt0++ = PCG_ATTR(0, 0, pal, 10);
		}
		else if (cell->note == XT_NOTE_CUT)
		{
			*nt0++ = PCG_ATTR(0, 0, pal, 'j');
			*nt0++ = PCG_ATTR(0, 0, pal, 'm');

			*nt0++ = PCG_ATTR(0, 0, pal, 9);

			*nt1++ = PCG_ATTR(0, 0, pal, 10);
			*nt0++ = PCG_ATTR(0, 0, pal, 10);
		}
		else
		{
			const uint8_t note = cell->note & XT_NOTE_TONE_MASK;
			*nt0++ = PCG_ATTR(0, 0, pal, note_labels[note * 2]);
			*nt0++ = PCG_ATTR(0, 0, pal, note_labels[1 + note * 2]);

			if (cell->note == XT_NOTE_NONE)
			{
				*nt0++ = PCG_ATTR(0, 0, pal, 9);

				*nt1++ = PCG_ATTR(0, 0, pal, 10);
				*nt0++ = PCG_ATTR(0, 0, pal, 10);
			}
			else
			{
				const uint8_t octave = cell->note >> 4;
				*nt0++ = PCG_ATTR(0, 0, pal, 0x01 + octave);

				// Instrument.
				const uint8_t instr_high = 0x70 + (cell->inst >> 4);
				const uint8_t instr_low = 0x70 + (cell->inst & 0x0F);
				*nt1++ = PCG_ATTR(0, 0, pal, instr_high);
				*nt0++ = PCG_ATTR(0, 0, pal, instr_low);
			}
		}
		*nt1++ = PCG_ATTR(0, 0, pal, 0x00);

		// Command 1.
		const uint16_t empty_cmd_column = PCG_ATTR(0, 0, pal, 0x0B);
		const uint16_t empty_arg_column = PCG_ATTR(0, 0, pal, 0x0C);
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
			*nt0++ = PCG_ATTR(0, 0, pal, cell->cmd1);
			*nt1++ = PCG_ATTR(0, 0, pal, arg_high);
			*nt0++ = PCG_ATTR(0, 0, pal, arg_low);
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
			*nt1++ = PCG_ATTR(0, 0, pal, cell->cmd2);
			*nt0++ = PCG_ATTR(0, 0, pal, arg_high);
			*nt1++ = PCG_ATTR(0, 0, pal, arg_low);
		}
		cell++;
		nt0 += (nt_width_cells - cell_width_cells);
		nt1 += (nt_width_cells - cell_width_cells);
	}
}

void xt_track_renderer_init(XtTrackRenderer *r)
{
	memset(r, 0, sizeof(*r));
	for (int i = 0; i < ARRAYSIZE(r->last_phrase_num); i++)
	{
		r->last_phrase_num[i] = 0xFFFF;
	}
	for (int i = 0; i < ARRAYSIZE(r->channel_dirty); i++)
	{
		r->channel_dirty[i] = 1;
	}

	// Load the palette.
	for (int i = 0; i < ARRAYSIZE(default_palette); i++)
	{
		x68k_vidcon_set_pcg_color(0x10 + i, default_palette[i]);
	}
}

void xt_track_renderer_repaint_channel(XtTrackRenderer *r, uint16_t channel)
{
	r->channel_dirty[channel] = 1;
}

void xt_track_renderer_tick(XtTrackRenderer *r, Xt *xt, uint16_t frame)
{
	int phrase_x_pos = 0;
	for (uint16_t i = 0; i < ARRAYSIZE(r->channel_dirty); i++)
	{
		const uint16_t phrase_id = xt_track_get_phrase_number_for_frame(&xt->track, i, frame);

		if (phrase_id != r->last_phrase_num[i])
		{
			r->last_phrase_num[i] = phrase_id;
			r->channel_dirty[i] = 1;
		}

		if (r->channel_dirty[i])
		{
			r->channel_dirty[i] = 0;
			if (i < 8)
			{
				xt_draw_fm_phrase(&xt->track.phrases[phrase_id],
				                  phrase_x_pos, xt->track.phrase_length);
			}
			else
			{
				// TODO: Draw PCM phrases (easy...)
			}
		}

		if (i < 8) phrase_x_pos += 7;
		else phrase_x_pos += 1;
	}
}
