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

#include "common.h"
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

static Xt xt;
static XtTrackRenderer renderer;

void set_demo_melodies(void)
{
	XtPhrase *ph = &xt.track.phrases[0];

	int cell_idx = 0;
	ph->cells[cell_idx++].note = XT_NOTE_C | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_C | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_C | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx++].note = XT_NOTE_C | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_F | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx++].note = XT_NOTE_G | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_A | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_F | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (2 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_C | (1 << 4);
	ph->cells[cell_idx++].note = XT_NOTE_OFF;

	ph += XT_PHRASES_PER_CHANNEL;
	cell_idx = 0;
	ph->cells[cell_idx].cmd1 = 'O';
	ph->cells[cell_idx].arg1 = 0x01;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_G | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_G | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_G | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_F | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_F | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_F | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_F | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_G | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph += XT_PHRASES_PER_CHANNEL;
	cell_idx = 0;
	ph->cells[cell_idx].cmd1 = 'O';
	ph->cells[cell_idx].arg1 = 0x11;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_E | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_E | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_E | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_D | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_D | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_D | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_D | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_E | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph += XT_PHRASES_PER_CHANNEL;
	cell_idx = 0;
	ph->cells[cell_idx].cmd1 = 'O';
	ph->cells[cell_idx].arg1 = 0x10;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_C | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_C | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_C | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_NONE;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_AS | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_CUT;
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_C | (5 << 4);
	ph->cells[cell_idx].inst = 0x01;
	ph->cells[cell_idx++].note = XT_NOTE_OFF;
}

void set_demo_instruments(void)
{
	XtInstrument *ins = &xt.track.instruments[0];

	// The bass from Private Eye (Daiginjou)
	ins->reg_20_pan_fl_con = 0xFB;
	ins->reg_38_pms_ams = 0;

	ins->reg_40_dt1_mul[0] = 8;
	ins->reg_60_tl[0] = 30;
	ins->reg_80_ks_ar[0] = 27;
	ins->reg_A0_ame_d1r[0] = 14;
	ins->reg_C0_dt2_d2r[0] = 0;
	ins->reg_E0_d1l_rr[0] = (3 << 4) | 10;

	ins->reg_40_dt1_mul[1] = 2;
	ins->reg_60_tl[1] = 45;
	ins->reg_80_ks_ar[1] = 31;
	ins->reg_A0_ame_d1r[1] = 12;
	ins->reg_C0_dt2_d2r[1] = 0;
	ins->reg_E0_d1l_rr[1] = (3 << 4) | 10;

	ins->reg_40_dt1_mul[2] = 0;
	ins->reg_60_tl[2] = 15;
	ins->reg_80_ks_ar[2] = 31;
	ins->reg_A0_ame_d1r[2] = 18;
	ins->reg_C0_dt2_d2r[2] = 0;
	ins->reg_E0_d1l_rr[2] = (5 << 4) | 10;

	ins->reg_40_dt1_mul[3] = 0;
	ins->reg_60_tl[3] = 6;
	ins->reg_80_ks_ar[3] = 31;
	ins->reg_A0_ame_d1r[3] = 5;
	ins->reg_C0_dt2_d2r[3] = 5;
	ins->reg_E0_d1l_rr[3] = (14 << 4) | 15;

	ins++;

	ins->reg_20_pan_fl_con = 0xC0 | (5 << 3) | 4;
	ins->reg_38_pms_ams = 0;

	ins->reg_40_dt1_mul[0] = 1;
	ins->reg_60_tl[0] = 11;
	ins->reg_80_ks_ar[0] = 30;
	ins->reg_A0_ame_d1r[0] = 8;
	ins->reg_C0_dt2_d2r[0] = 0;
	ins->reg_E0_d1l_rr[0] = (4 << 4) | 15;

	ins->reg_40_dt1_mul[1] = 1;
	ins->reg_60_tl[1] = 10;
	ins->reg_80_ks_ar[1] = 30;
	ins->reg_A0_ame_d1r[1] = 6;
	ins->reg_C0_dt2_d2r[1] = 0;
	ins->reg_E0_d1l_rr[1] = (4 << 4) | 15;

	ins->reg_40_dt1_mul[2] = 0;
	ins->reg_60_tl[2] = 22;
	ins->reg_80_ks_ar[2] = 30;
	ins->reg_A0_ame_d1r[2] = 11;
	ins->reg_C0_dt2_d2r[2] = 0;
	ins->reg_E0_d1l_rr[2] = (10 << 4) | 15;

	ins->reg_40_dt1_mul[3] = 0;
	ins->reg_60_tl[3] = 8;
	ins->reg_80_ks_ar[3] = 30;
	ins->reg_A0_ame_d1r[3] = 3;
	ins->reg_C0_dt2_d2r[3] = 0;
	ins->reg_E0_d1l_rr[3] = (0 << 4) | 15;
}

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

	xt_track_renderer_init(&renderer);

	// Set up xt with some test data
	memset((void *)&xt, 0, sizeof(xt));

	xt.track.num_phrases = 16;

	xt.track.num_frames = 4;
	xt.track.num_instruments = 1;

	xt.track.ticks_per_row = 6;
	xt.track.timer_period = 0xABCD;

	xt.track.phrase_length = 32;
	xt.track.loop_point = 1;

	xt_start_playing(&xt, 0, 0);

	set_demo_instruments();
	set_demo_melodies();

	for (int i = 0; i < 512; i++)
	{
		x68k_pcg_set_bg0_yscroll(xt.current_phrase_row * 8);
		x68k_pcg_set_bg1_yscroll(xt.current_phrase_row * 8);

		xt_tick(&xt);
		xt_update_opm_registers(&xt);

		x68k_wait_for_vsync();
		xt_track_renderer_tick(&xt, &renderer);
	}

//	opm_test();

	return 0;
}
