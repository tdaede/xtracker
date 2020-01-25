#include "xt_render.h"

#include "system/pcg.h"

#define PATTERN_DRAW_MAX_LENGTH 48

#define PAL_NOTE    1
#define PAL_INST    2
#define PAL_CMD     3
#define PAL_ARG     4

static inline const char *string_for_note(const XtNote note)
{
	const char *strings[] =
	{
		[XT_NOTE_NONE] = "--",
		[XT_NOTE_CS]   = "C#",
		[XT_NOTE_D]    = "D-",
		[XT_NOTE_DS]   = "D#",
		[XT_NOTE_E]    = "E-",
		[XT_NOTE_F]    = "F-",
		[XT_NOTE_FS]   = "F#",
		[XT_NOTE_G]    = "G-",
		[XT_NOTE_GS]   = "G#",
		[XT_NOTE_A]    = "A-",
		[XT_NOTE_AS]   = "A#",
		[XT_NOTE_B]    = "B-",
		[XT_NOTE_C]    = "C-",
	};
	if (note == XT_NOTE_OFF) return "==";
	if (note == XT_NOTE_CUT) return "^^";
	return strings[note & XT_NOTE_TONE_MASK];
}

static inline void draw_cell(const XtCell *cell, int x, int y)
{
	int8_t octave = (cell->note & XT_NOTE_OCTAVE_MASK) >> 4;
	// TODO: Alternate palette for no-key-on notes
	uint8_t inst = (cell->inst);

	const char *note_str = string_for_note(cell->note);

	pcg_set_bg1_tile(x + 0, y,  PCG_ATTR(0, 0, PAL_NOTE, note_str[0]));
	pcg_set_bg1_tile(x + 1, y, PCG_ATTR(0, 0, PAL_NOTE, note_str[1]));

	pcg_set_bg1_tile(x + 2, y, PCG_ATTR(0, 0, PAL_NOTE, '0' + octave));

	pcg_set_bg1_tile(x + 3, y, PCG_ATTR(0, 0, PAL_INST, '0' + (((inst & 0xF0) >> 4))));
	pcg_set_bg1_tile(x + 4, y, PCG_ATTR(0, 0, PAL_INST, '0' + (inst & 0xF)));

	if (cell->cmd1 == XT_CMD_NONE)
	{
		pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, PAL_CMD, '-'));
		pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, PAL_ARG, '-'));
		pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, PAL_ARG, '-'));
	}
	else
	{
		uint8_t arg = cell->arg1;
		pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, PAL_CMD, cell->cmd1));
		pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, PAL_ARG, '0' + ((arg & 0xF0) >> 4)));
		pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, PAL_ARG, '0' + (arg & 0xF)));
	}
	if (cell->cmd2 == XT_CMD_NONE)
	{
		pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, PAL_CMD, '-'));
		pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, PAL_ARG, '-'));
		pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, PAL_ARG, '-'));
	}
	else
	{
		uint8_t arg = cell->arg2;
		pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, PAL_CMD, cell->cmd2));
		pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, PAL_ARG,  '0' + ((arg & 0xF0) >> 4)));
		pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, PAL_ARG, '0' + (arg & 0xF)));
	}
}

void xt_draw_phrase(XtPhrase *phrase, int16_t x)
{
	pcg_set_bg1_tile(79, 0, PCG_ATTR(0, 0, PAL_NOTE, 'U'));
	draw_cell(&phrase->cells[0], x, 4);
	draw_cell(&phrase->cells[1], x, 5);
	draw_cell(&phrase->cells[2], x, 6);
}
