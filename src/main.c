#include <stdio.h>
#include <dos.h>
#include <iocs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "x68000/x68k_pcg.h"
#include "x68000/x68k_joy.h"
#include "x68000/x68k_crtc.h"
#include "x68000/x68k_vidcon.h"
#include "x68000/x68k_vbl.h"

#include "xt.h"
#include "xt_render.h"

void init(void)
{
	FILE *f;
	// Clear BG nametables
	x68k_vbl_wait_for_vblank();
	x68k_pcg_set_disp_en(0);
	memset((void *)PCG_SPR_TABLE, 0, 0x400);
	memset((void *)PCG_TILE_DATA, 0, 0x4000);
	memset((void *)PCG_BG0_NAME, 0, 0x2000);
	memset((void *)PCG_BG1_NAME, 0, 0x2000);

	// Set up BG layers
	x68k_pcg_set_bg0_txsel(0);
	x68k_pcg_set_bg1_txsel(1);
	x68k_pcg_set_bg0_enable(1);
	x68k_pcg_set_bg1_enable(1);
	x68k_pcg_set_lh(1);
	x68k_pcg_set_vres(0);
	x68k_pcg_set_hres(0);

	x68k_pcg_set_bg0_xscroll(0);
	x68k_pcg_set_bg1_xscroll(-4);

	x68k_pcg_set_bg0_yscroll(0);
	x68k_pcg_set_bg1_yscroll(0);

	x68k_pcg_clear_sprites();

	// Load font tileset
	f = fopen("font_4x8.bin", "rb");
	if (!f)
	{
		printf("Error: Could not load font_4x8.bin\n");
	}
	else
	{
		volatile uint16_t *pcg = (volatile uint16_t *)PCG_TILE_DATA;
		/*
		// 8x16 font
		for (uint16_t i = 0; i < 8192; i++)
		{
			volatile uint16_t *x68k_pcg_dest = pcg + i;
			uint16_t word = 0;
			word |= (fgetc(f) << 8);
			word |= fgetc(f);
			*x68k_pcg_dest = word;
		}*/

		// 8x8 font
		for (uint16_t i = 0; i < 2048; i++)
		{
			volatile uint16_t *x68k_pcg_dest = pcg + i;
			uint16_t word = 0;
			word |= (fgetc(f) << 8);
			word |= fgetc(f);
			*x68k_pcg_dest = word;
		}


		fclose(f);
	}

	x68k_pcg_set_disp_en(1);
}

void bg0print(const char *s, uint16_t x, uint16_t y, int pal)
{
	uint16_t x_orig = x;
	x68k_pcg_set_bg0_xscroll(0);
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
			x68k_pcg_set_bg0_tile(x, y, PCG_ATTR(0,0,pal,*s));
			s++;
			x++;
		}
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

static XtPhrase test_phrase;

int main(int argc, char **argv)
{
	_dos_super(0);
	_iocs_crtmod(0);
	_iocs_b_curoff();
	_iocs_g_clr_on();

	x68k_vbl_wait_for_vblank();
	x68k_crtc_init_default();
	x68k_vidcon_init_default();
	x68k_vidcon_commit_regs();
	x68k_pcg_init_default();

	init();

	printf("Some overlay text");

	x68k_vbl_wait_for_vblank();

	memset((void *)&test_phrase, 0, sizeof(test_phrase));

	for (int i = 0; i < 12; i++)
	{
		test_phrase.cells[i].note = (XT_NOTE_CS + i) | (0x3 << 4);
	}

	for (int i = 0; i < 512; i++)
	{
		x68k_wait_for_vsync();
		x68k_pcg_set_bg0_xscroll(-i);
		x68k_pcg_set_bg1_xscroll(-i -4);
		for (int j = 0; j < 5; j++) xt_draw_phrase(&test_phrase, 7 * j);
	}

	opm_test();

	return 0;
}
