/*

X68000 Video Controller Helper Functions (vidcon)
c. Michael Moffitt 2017

Provides some helper functions for configuring the video controller. The
configuration is stored in three words of shadow copies of the registers,
which are initially set with x68k_vidcon_init_default().

Afterwards, individual bits may be set or cleared with the clearly named
helper functions. These functions modify the shadow copies. To write the shadow
copies to the actual hardware registers, call x68k_vidcon_commit_regs().

*/

#ifndef X68K_VIDCON_H
#define X68K_VIDCON_H

#include <stdint.h>

// RGB palette entry macro
#define PAL_RGB5(r, g, b) ( (((r) & 0x1F) << 6) | (((g) & 0x1F) << 11) | (((b) & 0x1F) << 1) )
#define PAL_RGB4(r, g, b) ( (((r << 1) & 0x1F) << 6) | (((g << 1) & 0x1F) << 11) | (((b << 1) & 0x1F) << 1) )
#define PAL_RGB8(r, g, b) ( (((r >> 3) & 0x1F) << 6) | (((g >> 3) & 0x1F) << 11) | (((b >> 3) & 0x1F) << 1) )

// Set up a sane default configuration
// 512x512, 16-color
// Sprites > Text > GP0 > GP1 > GP2 > GP3
// Enable all graphics layers.
// The other wacky shit is not enabled.
void x68k_vidcon_init_default(void);

// Update hardware registers from shadow copies
void x68k_vidcon_commit_regs(void);

// Palette Writes ============================================================

// Graphics plane palette entries
static inline void x68k_vidcon_set_gp_color(uint8_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)0xE82000;
	p += index;
	*p = val;
}

// Text plane color entries
static inline void x68k_vidcon_set_text_color(uint8_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)0xE82200;
	p += index;
	*p = val;
}

// Sprite palette entries
static inline void x68k_vidcon_set_pcg_color(uint8_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)0xE82200;
	p += index;
	*p = val;
}

// R0: screen ================================================================

// Set screen size between 512px (0) and 1024px(1)
void x68k_vidcon_set_screen_size(uint8_t screen_size);

// Set color mode between 16(0), 256(1), and 65536(3)
void x68k_vidcon_set_color_mode(uint8_t color_mode);

// R1: priorities ============================================================

// Set priorities of the graphics layers, where 0 is the topmost
void x68k_vidcon_set_prio_pcg(uint8_t priority);
void x68k_vidcon_set_prio_text(uint8_t priority);
void x68k_vidcon_set_prio_graphics(uint8_t priority);

// Set priorities within GVRAM-based layers
void x68k_vidcon_set_prio_gp0(uint8_t priority);
void x68k_vidcon_set_prio_gp1(uint8_t priority);
void x68k_vidcon_set_prio_gp2(uint8_t priority);
void x68k_vidcon_set_prio_gp3(uint8_t priority);

// R2: misc. settings, enable masks ==========================================

// Do not display image even when superimposing
void x68k_vidcon_set_video_cut(uint8_t cut_en);

// Set translucent: text palette 0 (force translucent regardless of exon, etc)
void x68k_vidcon_set_ah(uint8_t ah_en);

// Set translucent: video image (used in color image unit)
void x68k_vidcon_set_vht(uint8_t vht_en);

// Enable special priority / translucent mode
void x68k_vidcon_set_exon(uint8_t exon_en);

// Special priority (0) or translucent mode (1) selection
void x68k_vidcon_set_hp(uint8_t hp);

// Sharp reserved(0) or something else(1)
void x68k_vidcon_set_bp(uint8_t bp);

// Set translucent: graphic screen
void x68k_vidcon_set_gg(uint8_t gg_en);

// Set translucent: text/sprite screen
void x68k_vidcon_set_gt(uint8_t gt_en);

// Enable border color display
void x68k_vidcon_border_enable(uint8_t en);

// Enable sprite display
void x68k_vidcon_pcg_enable(uint8_t en);

// Enable text plane display
void x68k_vidcon_text_enable(uint8_t en);

// Enable graphic plane display
void x68k_vidcon_graphic_enable(uint8_t en);

// Enable particular graphic layers
void x68k_vidcon_graphic_gp0_enable(uint8_t en);
void x68k_vidcon_graphic_gp1_enable(uint8_t en);
void x68k_vidcon_graphic_gp2_enable(uint8_t en);
void x68k_vidcon_graphic_gp3_enable(uint8_t en);

#endif // X68K_VIDCON_H
