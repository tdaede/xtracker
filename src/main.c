#include <stdio.h>
#include <dos.h>
#include <iocs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "system/pcg.h"
#include "system/joy.h"
#include "system/crtc.h"
#include "system/vidcon.h"
#include "system/vbl.h"

#include "xt_render.h"

void init(void)
{
	FILE *f;
	// Clear BG nametables
	wait_for_vblank();
	pcg_set_disp_en(0);
	for (uint16_t y = 0; y < 64; y++)
	{
		for (uint16_t x = 0; x < 64; x++)
		{
			volatile uint16_t *p = (volatile uint16_t *)(0xEBC000);
			p += x;
			p += (y << 6);
			*p = PCG_ATTR(0, 0, 0, 0);
		}
	}

	// Clear BG tile 0, at least
	for (uint16_t i = 0; i < 64; i++)
	{
		volatile uint16_t *pcg = (volatile uint16_t *)PCG_TILE_DATA;
		pcg += i;
		*pcg = 0;
	}

	// Set up BG layers
	pcg_set_bg0_txsel(0);
	pcg_set_bg1_txsel(1);
	pcg_set_bg0_enable(1);
	pcg_set_bg1_enable(1);
	pcg_set_lh(1);
	pcg_set_vres(0);
	pcg_set_hres(0);

	// Load font tileset
	f = fopen("font.bin", "rb");
	if (!f)
	{
		printf("Error: Could not load font.bin.A\n");
	}
	else
	{
		volatile uint16_t *pcg = (volatile uint16_t *)PCG_TILE_DATA;
		/*
		// 8x16 font
		for (uint16_t i = 0; i < 8192; i++)
		{
			volatile uint16_t *pcg_dest = pcg + i;
			uint16_t word = 0;
			word |= (fgetc(f) << 8);
			word |= fgetc(f);
			*pcg_dest = word;
		}*/

		// 8x8 font
		for (uint16_t i = 0; i < 2048; i++)
		{
			volatile uint16_t *pcg_dest = pcg + i;
			uint16_t word = 0;
			word |= (fgetc(f) << 8);
			word |= fgetc(f);
			*pcg_dest = word;
		}


		fclose(f);
	}

	pcg_set_disp_en(1);
}

void bg0print(const char *s, uint16_t x, uint16_t y, int pal)
{
	uint16_t x_orig = x;
	pcg_set_bg0_xscroll(0);
	while(*s)
	{
		if (*s == '\n')
		{
			y++;
			x = x_orig;
			s++;
		}
		else
		{
			pcg_set_bg0_tile(x, y, PCG_ATTR(0,0,pal,*s));
			s++;
			x++;
		}
	}
}

static void movement_test(void)
{
	pcg_set_disp_en(1);

	for (uint16_t i = 0; i < 600; i++)
	{
		wait_for_vblank();
		pcg_clear_sprites();
		pcg_set_sprite(0, i, 0, PCG_ATTR(0, 0, 1, 1), 0x03);
		pcg_set_bg0_tile(8, 8, PCG_ATTR(0,0,1,('0' + _iocs_joyget(0))));
		x68k_opm_set_key_on(0, 0);
		wait_for_vdisp();
	}
}

static void opm_test(void)
{
	for (int i = 0; i < 8; i++)
	{
		x68k_opm_set_key_on(i, 0);
		x68k_opm_set_lr_fl_con(i, X68K_OPM_PAN_BOTH_ENABLE, 0, 1);
		switch(i)
		{
			default:
				continue;
			case 0:
				x68k_opm_set_oct_note(i, 4, OPM_NOTE_C);
				break;
			case 1:
				x68k_opm_set_oct_note(i, 4, OPM_NOTE_E);
				break;
			case 2:
				x68k_opm_set_oct_note(i, 4, OPM_NOTE_G);
				break;
			case 3:
				x68k_opm_set_oct_note(i, 4, OPM_NOTE_B);
				break;
			case 4:
				x68k_opm_set_oct_note(i, 5, OPM_NOTE_D);
				break;
		}

		for (int j = 0; j < 4; j++)
		{
			x68k_opm_set_d1t_mul(i, j, 4, 1);
			x68k_opm_set_tl(i, j, 16);
			x68k_opm_set_ks_ar(i, j, 2, 14);
			x68k_opm_set_ame_d1r(i, j, 0, 3);
			x68k_opm_set_dt2_d2r(i, j, 1, 2);
			x68k_opm_set_d1l_rr(i, j, 7, 7);
		}
		x68k_opm_set_key_on(i, 0xF);
	}
}

static void load_test_sprite(void)
{
	// Load the sprite from "sprite.bin"
	FILE *f = fopen("sprite.bin", "rb");

	if (!f)
	{
		printf("Error: Couldn't load file.\n");
		return;
	}

	volatile uint16_t *pcg = (volatile uint16_t *)PCG_TILE_DATA;

	pcg_set_disp_en(0);
	for (int i = 0; i < 64; i++)
	{
		volatile uint16_t *pcg_dest = pcg + 64;
		uint16_t word = 0;
		word |= (fgetc(f) << 8);
		word |= fgetc(f);
		pcg_dest += i;
		*pcg_dest = word;
	}
	// Set a palette
	vidcon_set_spr_color(0, PALRGB(2, 2, 2));
	vidcon_set_spr_color(1, PALRGB(31, 31, 31));
	vidcon_set_spr_color(2, PALRGB(31, 0, 0));
	vidcon_set_spr_color(3, PALRGB(3, 3, 28));

	fclose(f);
	f = NULL;
}

int main(int argc, char **argv)
{
	_dos_super(0);
	_iocs_crtmod(0);
	_iocs_b_curoff();
	_iocs_g_clr_on();

	wait_for_vblank();

	crtc_init_default();
	vidcon_init_default();
	vidcon_commit_regs();
	pcg_init_default();

	init();
	load_test_sprite();

	wait_for_vblank();

	XtPhrase test_phrase;
	memset((void *)&test_phrase, 0, sizeof(test_phrase));

	test_phrase.cells[0].note = XT_NOTE_CS | (0x3 << 4);
	test_phrase.cells[0].inst = 3;
	test_phrase.cells[0].cmd1 = 'V';
	test_phrase.cells[0].arg1 = 0x69;
	test_phrase.cells[0].cmd2 = 'B';
	test_phrase.cells[0].arg2 = 0x1;

	test_phrase.cells[1].note = XT_NOTE_B | (0x06 << 4);
	test_phrase.cells[1].inst = 2;
	test_phrase.cells[1].cmd1 = '1';
	test_phrase.cells[1].arg1 = 0x13;
	test_phrase.cells[1].cmd2 = '2';
	test_phrase.cells[1].arg2 = 0x37;

	xt_draw_phrase(&test_phrase, 2);

	opm_test();

	movement_test();

	return 0;
}
