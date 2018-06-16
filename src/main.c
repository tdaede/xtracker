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

#include "render_pattern.h"

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

	volatile uint8_t *kctrl = (volatile uint8_t *)0xE8E007;

	*kctrl = 0x0E;

	for (uint16_t i = 0; i < 600; i++)
	{
		wait_for_vblank();
		pcg_clear_sprites();
		pcg_set_sprite(0, i, 0, PCG_ATTR(0, 0, 1, 1), 0x03);
		pcg_set_bg0_tile(8, 8, PCG_ATTR(0,0,1,('0' + _iocs_joyget(0))));
		wait_for_vdisp();
	}
}

int main(int argc, char **argv)
{
	_dos_super(0);
	_iocs_crtmod(4);
	_iocs_b_curoff();
	_iocs_g_clr_on();
	volatile uint16_t *pcg = (volatile uint16_t *)PCG_TILE_DATA;
	FILE *f;

	// Set up video registers

	wait_for_vblank();

	crtc_init_default();
	vidcon_init_default();
	vidcon_commit_regs();
	pcg_init_default();
	init();

	// Load the sprite from "sprite.bin"
	f = fopen("sprite.bin", "rb");

	if (!f)
	{
		printf("Error: Couldn't load file.\n");
		return 0;
	}


	for (int i = 0; i < 64; i++)
	{
		volatile uint16_t *pcg_dest = pcg + 64;
		uint16_t word = 0;
		word |= (fgetc(f) << 8);
		word |= fgetc(f);
		pcg_dest += i;
		*pcg_dest = word;
	}

	wait_for_vblank();
	pcg_set_disp_en(0);
	// Set a palette
	vidcon_set_spr_color(0, PALRGB(2, 2, 2));
	vidcon_set_spr_color(1, PALRGB(31, 31, 31));
	vidcon_set_spr_color(2, PALRGB(31, 0, 0));
	vidcon_set_spr_color(3, PALRGB(3, 3, 28));
	for (int i = 0; i < 128; i++)
	{
		vidcon_set_spr_color(i, PALRGB(i % 32, i % 16, 24 + (i % 8)));
	}

	PatternCell test_pattern[64];
	test_pattern[0].note = 32;
	test_pattern[0].inst = 3;
	test_pattern[0].cmd1 = 'V';
	test_pattern[0].arg1 = 0x69;
	test_pattern[0].cmd2 = 'B';
	test_pattern[0].arg2 = 0x1;
	test_pattern[4].note = 16;
	test_pattern[4].inst = 2;
	test_pattern[4].cmd1 = 'X';
	test_pattern[4].arg1 = 0x13;
	test_pattern[4].cmd2 = 'A';
	test_pattern[4].arg2 = 0x37;

	draw_pattern(test_pattern, 2);


	// Enable
	fclose(f);
	f = NULL;

	movement_test();

	// Place the sprite
	return 0;
}
