#include "x68000/x68k_pcg.h"

static uint16_t x68k_pcg_mode, x68k_pcg_ctrl;
static volatile uint16_t *x68k_pcg_mode_r = (volatile uint16_t *)PCG_MODE;
static volatile uint16_t *x68k_pcg_ctrl_r = (volatile uint16_t *)PCG_BG_CTRL;

/*
Control:    0xEB0808
---- --9- ---- ---- D/C          turn on PCG to allow PCG register access
---- ---- --54 ---- BG1 TXsel    nametable mapping
---- ---- ---- 3--- BG1 ON
---- ---- ---- -21- BG0 TXsel    nametable mapping
---- ---- ---- ---0 BG0 ON
H-total:    0xEB080A
---- ---- 7654 3210 H total
H-disp:     0xEB080C
---- ---- --54 3210 H disp
V-disp:     0xEB080E
---- ---- 7654 3210 V disp
Mode:       0xEB0810
---- ---- ---4 ---- L/H          15KHz(0), 31Khz(1)
---- ---- ---- 32-- V-res
---- ---- ---- --10 H-res        If nonzero, enables 16x16 tiles

*/

// Set up with some sane defaults.
void x68k_pcg_init_default(void)
{
	x68k_pcg_mode = 0;
	x68k_pcg_ctrl = 0;
	x68k_pcg_set_bg1_txsel(1);
	x68k_pcg_set_bg0_txsel(0);
	x68k_pcg_set_bg0_xscroll(0);
	x68k_pcg_set_bg0_yscroll(0);
	x68k_pcg_set_bg1_xscroll(0);
	x68k_pcg_set_bg1_yscroll(0);
	x68k_pcg_set_bg0_enable(1);
	x68k_pcg_set_bg1_enable(1);
	x68k_pcg_set_lh(1);
	x68k_pcg_set_vres(1);
	x68k_pcg_set_hres(1);
	x68k_pcg_clear_sprites();
	x68k_pcg_set_disp_en(1);
}

// Set to CPU(1) or display(0)
void x68k_pcg_set_disp_en(uint8_t en)
{
	x68k_pcg_ctrl &= ~(0x0200);
	x68k_pcg_ctrl |= (en ? 0x0200 : 0x0000);
	*x68k_pcg_ctrl_r = x68k_pcg_ctrl;
}

// Change the mappings for BG1 and BG0 nametables
void x68k_pcg_set_bg1_txsel(uint8_t t)
{
	x68k_pcg_ctrl &= ~(0x0030);
	x68k_pcg_ctrl |= (t & 0x03) << 4;
	*x68k_pcg_ctrl_r = x68k_pcg_ctrl;
}
void x68k_pcg_set_bg0_txsel(uint8_t t)
{
	
	x68k_pcg_ctrl &= ~(0x0030);
	x68k_pcg_ctrl |= (t & 0x03) << 1;
	*x68k_pcg_ctrl_r = x68k_pcg_ctrl;
}

// Enable or disable BG layer display
void x68k_pcg_set_bg1_enable(uint8_t en)
{
	x68k_pcg_ctrl &= ~(0x0008);
	x68k_pcg_ctrl |= (en ? 0x08 : 0x000);
	*x68k_pcg_ctrl_r = x68k_pcg_ctrl;
}
void x68k_pcg_set_bg0_enable(uint8_t en)
{
	x68k_pcg_ctrl &= ~(0x0001);
	x68k_pcg_ctrl |= (en ? 0x001 : 0x000);
	*x68k_pcg_ctrl_r = x68k_pcg_ctrl;
}

void x68k_pcg_set_ctrl(uint16_t ctrl)
{
	x68k_pcg_mode = ctrl;
	*x68k_pcg_mode_r = ctrl;
}

void x68k_pcg_set_lh(uint8_t lh)
{
	x68k_pcg_mode &= ~(0x0010);
	x68k_pcg_mode |= (lh ? 0x10 : 0x00);
	*x68k_pcg_mode_r = x68k_pcg_mode;
}

void x68k_pcg_set_vres(uint8_t v)
{
	x68k_pcg_mode &= ~(0x0000C);
	x68k_pcg_mode |= ((v & 0x03) << 2);
	*x68k_pcg_mode_r = x68k_pcg_mode;
}
void x68k_pcg_set_hres(uint8_t h)
{
	x68k_pcg_mode &= ~(0x00003);
	x68k_pcg_mode |= (h & 0x03);
	*x68k_pcg_mode_r = x68k_pcg_mode;
}
