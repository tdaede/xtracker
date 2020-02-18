#ifndef _X68K_PCG_H
#define _X68K_PCG_H

#include <stdint.h>

// Memory map
#define PCG_SPR_TABLE  0xEB0000
#define PCG_TILE_DATA  0xEB8000
#define PCG_BG0_NAME   0xEBC000
#define PCG_BG1_NAME   0xEBE000
#define PCG_BG0_XSCRL  0xEB0800
#define PCG_BG0_YSCRL  0xEB0802
#define PCG_BG1_XSCRL  0xEB0804
#define PCG_BG1_YSCRL  0xEB0806
#define PCG_BG_CTRL    0xEB0808
#define PCG_HTOTAL     0xEB080A
#define PCG_HDISP      0xEB080C
#define PCG_VDISP      0xEB080E
#define PCG_MODE       0xEB0810

/*

A lot of this is based on the X68000 Technical Data Book.

Tile VRAM =====================================================================

Tile graphics data is stored at 0xEB8000, and is of size 0x4000.

Sprites =======================================================================

Sprite table begins at 0xEB0000, and each sprite is 4 bytes.
Maximum 128 sprites.

Each sprite:
0x0:
---- ---8 7654 3210   X Position
0x2:
---- ---8 7654 3210   Y position
0x4:
FE-- ---- ---- ----   V/H Reflect
---- BA98 ---- ----   Color
---- ---- 7654 3210   Pattern
0x6:
---- ---- ---- -210   Layer Priority

See the struct X68kPcgSprite.

Backgrounds ===================================================================

Nametables begin at 0xEBC000 and 0xEBE000 for BG0 and BG1, respectively.
Scroll registers are at EB0800

Background scroll registers are at:
BG0: 0xEB0800
BG1: 0xEB0804

Each scroll register:
0x0:
---- --XX XXXX XXXX   X position
0x2:
---- --YY YYYY YYYY   Y position

BG control register is at 0xEB0808.

BGControl register:
---- --9- ---- ---- Display Enable. Turn off to modify registers.
---- ---- --54 ---- BG1 textsel - nametable mapping (0xEBC000 vs 0xEBE000)
---- ---- ---- 3--- BG1 ON
---- ---- ---- -21- BG0 textsel - nametable mapping (0xEBC000 vs 0xEBE000)
---- ---- ---- ---0 BG0 ON

Control Register ==============================================================

The control register is at 0xEB080A.

0x0:
---- ---- 7654 3210 H total
0x2:
---- ---- --54 3210 H disp
0x4:
---- ---- 7654 3210 V disp
0x6:
---- ---- ---4 ---- L/H        15KHz (0), 31Khz (1)
---- ---- ---- 32-- V-res      256-line mode (00), 512-line mode (01)
---- ---- ---- --10 H-res      256-dot mode (00), 512-dot mode (01)

Tile size (8x8 or 16x16) is influenced by HRES in 0xEB0810
    Display mode    Screen size     Layer size          Tile size
    register HRES
    bits
    0 0             256 x 256 dots  512 x 512 dots      8 x 8 dots
    0 1             512 x 512 dots  1024 x 1024 dots    16 x 16 dots

Background nametables =========================================================

Layout data for the Background planes start at 0xEBC000 and 0xEBE000 for BG0 and
BG1, respectively, and are sied 0x2000.

Tile format:
f--- ---- ---- ---- y flip
-e-- ---- ---- ---- x flip
--dc ---- ---- ---- unused
---- ba98 ---- ---- color
---- ---- 7654 4310 pattern #

Tile indexing =================================================================

In 16x16 tile mode (hres on) tiles are "as you expect".
16x16 tiles are indexed within PCG memory like so:

00 -> 01 -> 02 -> 03 -> 04 ...

16 -> 17 -> 18 -> 19 -> 20 ...

In 8x8 tile mode (hres off) tiles are in a zig-zag order. They still follow
addressing rules of the 16x16 mode in a sense.
8x8 tiles are indexed within PCG memory like so:

00  02  03  05  08
| / | / | / | / |
01  03  05  07  ...

64  66  68
| / | / |
65  67  ...

*/

typedef enum X68kPcgRes
{
	PCG_RES_LOW = 0,
	PCG_RES_HIGH = 1,
} X68kPcgRes;

// Attributes to specify sprite and backdrop tiles
#define PCG_ATTR(_yf_,_xf_,_c_,_p_) ((((_yf_)&1)<<15) | (((_xf_)&1)<<14) |\
                                    (((_c_)&0xF)<<8) | (((_p_)&0xFF)))

// Struct representing a sprite
typedef struct
{
	uint16_t x; // Limited to 10 bits of precision
	uint16_t y; // "
	uint16_t attr; // Bitfield; see PCG_ATTR macro
	uint16_t prio; // Priority relative to BG
} X68kPcgSprite;

/* Priority between sprites and PCG backgrounds:

Sprite offset +06 is priority, or "PRW" as Inside calls it.

PRW     Layering
----------------
0 0     Sprites are not drawn.
0 1     Sprites are behind both tile layers.
1 0     Sprites are behind BG0, but in front of BG1.
1 1     Sprites are above both BG layers.

*/

// Setup =====================================================================

// Set up with some sane defaults.
void x68k_pcg_init_default(void);

// Turn off the display for faster transfer
void x68k_pcg_set_disp_en(uint8_t en);

// Change the mappings for BG1 and BG0 nametables
void x68k_pcg_set_bg1_txsel(uint8_t t);
void x68k_pcg_set_bg0_txsel(uint8_t t);

// Enable or disable BG layer display
void x68k_pcg_set_bg1_enable(uint8_t en);
void x68k_pcg_set_bg0_enable(uint8_t en);

// Configure display timings; should correspond with CRTC.

// t: 0 - 255
static inline void x68k_pcg_set_htotal(uint8_t t)
{
	volatile uint16_t *r = (volatile uint16_t *)PCG_HTOTAL;
	*r = t;
}

// t: 0 - 64
static inline void x68k_pcg_set_hdisp(uint8_t t)
{
	volatile uint16_t *r = (volatile uint16_t *)PCG_HDISP;
	*r = t;
}

// t : 0 - 255
static inline void x68k_pcg_set_vdisp(uint8_t t)
{
	volatile uint16_t *r = (volatile uint16_t *)PCG_VDISP;
	*r = t;
}


void x68k_pcg_set_ctrl(uint16_t ctrl);

// Set low/high frequency.
// lh: 1 = high; 0 = low
void x68k_pcg_set_lh(uint8_t lh);

// Vertical and horizontal resolution; seems related to sprite and tile size
void x68k_pcg_set_vres(uint8_t v);
void x68k_pcg_set_hres(uint8_t h);

// Data manipulation =========================================================

// Sets a tile. Not recommended for drawing a background or much else beyond
// small changes or playing around, as it's slower than doing a large DMA.
static inline void x68k_pcg_set_bg0_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)PCG_BG0_NAME;
	nt += x;
	nt += (y << 6);
	*nt = attr;
}

static inline void x68k_pcg_set_bg1_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)PCG_BG1_NAME;
	nt += x;
	nt += (y << 6);
	*nt = attr;
}

// Scroll registers
static inline void x68k_pcg_set_bg0_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)PCG_BG0_XSCRL;
	*scr = x;
}

static inline void x68k_pcg_set_bg1_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)PCG_BG1_XSCRL;
	*scr = x;
}

static inline void x68k_pcg_set_bg0_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)PCG_BG0_YSCRL;
	*scr = y;
}

static inline void x68k_pcg_set_bg1_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)PCG_BG1_YSCRL;
	*scr = y;
}

// Does pointer math to get a handle to a sprite entry
static inline volatile X68kPcgSprite *x68k_pcg_get_sprite(uint8_t idx)
{
	volatile X68kPcgSprite *ret = (volatile X68kPcgSprite *)PCG_SPR_TABLE;
	ret += idx;
	return ret;
}

// Semi-pointless sprite setter function
static inline void x68k_pcg_set_sprite(uint8_t idx, int16_t x,
                                       int16_t y, uint16_t attr, uint16_t prio)
{
	volatile X68kPcgSprite *spr = x68k_pcg_get_sprite(idx);
	spr->x = x + 16;
	spr->y = y + 16;
	spr->attr = attr;
	spr->prio = prio;
}

// Clears all sprites by setting priority to %00 (invisible)
static inline void x68k_pcg_clear_sprites(void)
{
	volatile X68kPcgSprite *spr = x68k_pcg_get_sprite(0);
	uint8_t i = 0;
	for (i = 0; i < 128; i++)
	{
		spr[i].prio = 0x00;
	}
}

// Sprite drawing routines using an internal last-sprite variable.
void x68k_pcg_add_sprite(int16_t x, int16_t y, uint16_t attr, uint16_t prio);
void x68k_pcg_finish_sprites(void);

#endif
