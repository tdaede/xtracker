#include "render_pattern.h"

#include "system/pcg.h"

#define PATTERN_DRAW_MAX_LENGTH 48

#define PAL_NOTE    1
#define PAL_INST    2
#define PAL_EFFECT  3
#define PAL_EFFECT2 3

static const char *note_strings[] =
{
	"A-",
	"A#",
	"B-",
	"C-",
	"C#",
	"D-",
	"D#",
	"E-",
	"F-",
	"F#",
	"G-",
	"G#",
};

static inline void draw_cell(PatternCell *cell, int x, int y)
{
	if (cell)
	{
		if (cell->note == NOTE_NONE)
		{
			pcg_set_bg1_tile(x, y, PCG_ATTR(0, 0, PAL_NOTE, '-'));
			pcg_set_bg1_tile(x + 1, y, PCG_ATTR(0, 0, PAL_NOTE, '-'));
			pcg_set_bg1_tile(x + 2, y, PCG_ATTR(0, 0, PAL_NOTE, '-'));
			pcg_set_bg1_tile(x + 3, y, PCG_ATTR(0, 0, PAL_INST, '-'));
			pcg_set_bg1_tile(x + 4, y, PCG_ATTR(0, 0, PAL_INST, '-'));
			pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
			pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
			pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
			pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
			pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
			pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
		}
		else if (cell->note >= NOTE_VALID)
		{
			int8_t tone = (cell->note - 1) % 12;
			int8_t octave = (cell->note - 1) / 12;
			uint8_t inst = (cell->inst);
			const char *note_str = note_strings[tone];
			pcg_set_bg1_tile(x, y, PCG_ATTR(0, 0, PAL_NOTE,
			                                note_str[0]));
			pcg_set_bg1_tile(x + 1, y, PCG_ATTR(0, 0, PAL_NOTE,
			                                    note_str[1]));
			pcg_set_bg1_tile(x + 2, y, PCG_ATTR(0, 0, PAL_NOTE,
			                                    '0' + octave));

			pcg_set_bg1_tile(x + 3, y, PCG_ATTR(0, 0, PAL_INST,
			                                    '0' + (((inst & 0xF0) >> 4))));
			pcg_set_bg1_tile(x + 4, y, PCG_ATTR(0, 0, PAL_INST,
			                                    '0' + (inst & 0xF)));

			if (cell->cmd1 == EFFECT_NONE)
			{
				pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
				pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
				pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, PAL_EFFECT, '-'));
			}
			else
			{
				uint8_t arg = cell->arg1;
				pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, PAL_EFFECT, cell->cmd1));
				pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, PAL_EFFECT,
				                                 '0' + ((arg & 0xF0) >> 4)));
				pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, PAL_EFFECT,
				                                 '0' + (arg & 0xF)));
			}
			if (cell->cmd2 == EFFECT_NONE)
			{
				pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
				pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
				pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, PAL_EFFECT2, '-'));
			}
			else
			{
				uint8_t arg = cell->arg2;
				pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, PAL_EFFECT, cell->cmd2));
				pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, PAL_EFFECT,
				                                 '0' + ((arg & 0xF0) >> 4)));
				pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, PAL_EFFECT,
				                                  '0' + (arg & 0xF)));
			}
		}
	}
	else
	{
		pcg_set_bg1_tile(x, y, PCG_ATTR(0, 0, 0, '-'));
		pcg_set_bg1_tile(x + 1, y, PCG_ATTR(0, 0, 0, ' '));
		pcg_set_bg1_tile(x + 2, y, PCG_ATTR(0, 0, 0, 'N'));
		pcg_set_bg1_tile(x + 3, y, PCG_ATTR(0, 0, 0, 'O'));
		pcg_set_bg1_tile(x + 4, y, PCG_ATTR(0, 0, 0, 'T'));
		pcg_set_bg1_tile(x + 5, y, PCG_ATTR(0, 0, 0, 'H'));
		pcg_set_bg1_tile(x + 6, y, PCG_ATTR(0, 0, 0, 'I'));
		pcg_set_bg1_tile(x + 7, y, PCG_ATTR(0, 0, 0, 'N'));
		pcg_set_bg1_tile(x + 8, y, PCG_ATTR(0, 0, 0, 'G'));
		pcg_set_bg1_tile(x + 9, y, PCG_ATTR(0, 0, 0, ' '));
		pcg_set_bg1_tile(x + 10, y, PCG_ATTR(0, 0, 0, '-'));
	}
}

void draw_pattern(PatternCell *pattern, int16_t x)
{
	int pattern_len = PATTERN_LEN; // TODO: Should poll the track metadata
	for (uint16_t i = 0; i < pattern_len; i++)
	{
		PatternCell *p = (i >= 0 && i < pattern_len) ? (pattern + i) : 0;
		draw_cell(p, x, i);
	}
}
