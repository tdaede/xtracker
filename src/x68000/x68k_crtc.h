/*

// TODO: Page 224 of Inside has a good example of the raster copy

X68000 CRT Controller Helper Functions (crtc)
c. Michael Moffitt 2017

Provides some helper functions for configuring the CRT controller. Most writes
are not shadow copied as the registers represent a single word or less of data.

R20, R21, and R22 are shadowed, but unlike the vidcon helpers these are updated
immediately and consequently do not require calls to a commit function.

x68k_crtc_init_default does not touch display timings, and only resets scroll for
all layers. R20 and R21 are set up with what I would hope are sane defaults,
but this only affects the shadow copies, which will only be committed if those
registers are touched.

*/
#ifndef _68K_CRTC_H
#define _68K_CRTC_H

#include <stdint.h>

// VRAM memory mapping
#define GVRAM_BASE ((uint8_t *)0xC00000)
#define TVRAM_BASE ((uint8_t *)0xE00000)

// Set up some sane defaults
// TODO: Verify on hardware
void x68k_crtc_init_default(void);

// R00 - R08: Display timings ================================================

// R00: Total horizontal period
void x68k_crtc_set_htotal(uint16_t v);

// R01: HSync end position
void x68k_crtc_set_hsync_end(uint16_t v);

// R02: H Display start
void x68k_crtc_set_hdisp_start(uint16_t v);

// R03: H Display end
void x68k_crtc_set_hdisp_end(uint16_t v);

// R04: Total vertical period
void x68k_crtc_set_vtotal(uint16_t v);

// R05: VSync end position
void x68k_crtc_set_vsync_end(uint16_t v);

// R06: V Display start
void x68k_crtc_set_vdisp_start(uint16_t v);

// R07: V Display end
void x68k_crtc_set_vdisp_end(uint16_t v);

// R08 - R09: Misc ===========================================================

// R08: External sync horizontal phase fine adjustment
void x68k_crtc_set_horiz_adj(uint16_t v);

// R09: Raster number for raster interrupt
void x68k_crtc_set_raster_interrupt(uint16_t v);

// R10 - R19: Scroll registers ===============================================

// R10: Text layer X scroll
void x68k_crtc_set_text_xscroll(uint16_t v);

// R11: Text layer Y scroll
void x68k_crtc_set_text_yscroll(uint16_t v);

// R12: Graphic layer 0 X scroll
void x68k_crtc_set_gp0_xscroll(uint16_t v);

// R13: Graphic layer 0 Y scroll
void x68k_crtc_set_gp0_yscroll(uint16_t v);

// R14: Graphic layer 1 X scroll
void x68k_crtc_set_gp1_xscroll(uint16_t v);

// R15: Graphic layer 1 Y scroll
void x68k_crtc_set_gp1_yscroll(uint16_t v);

// R16: Graphic layer 2 X scroll
void x68k_crtc_set_gp2_xscroll(uint16_t v);

// R17: Graphic layer 2 Y scroll
void x68k_crtc_set_gp2_yscroll(uint16_t v);

// R18: Graphic layer 3 X scroll
void x68k_crtc_set_gp3_xscroll(uint16_t v);

// R19: Graphic layer 3 Y scroll
void x68k_crtc_set_gp3_yscroll(uint16_t v);

// R20 - R23: Misc 2 =========================================================

// Set T-VRAM usage to display(0) or buffer(1)
void x68k_crtc_set_tvram_usage(uint8_t v);

// Set G-VRAM usage to display(0) or buffer(1)
void x68k_crtc_set_gvram_usage(uint8_t v);

// Set screen size to 512px(0) or 1024px(1)
void x68k_crtc_set_screen_size(uint8_t v);

// Set color mode between 16(0), 256(1), and 65536(3)
void x68k_crtc_set_color_mode(uint8_t v);

// Set horizontal deflection frequency between 15.98KHz(0) and 31.50KHz(1)
void x68k_crtc_set_horizontal_freq(uint8_t v);

// Set vertical display size between 256(0), 512(1), or 1024(2) if Hf=31.5Khz
void x68k_crtc_set_display_height(uint8_t v);

// Set horizontal display size between 256(0), 512(1), 768(2), or 50MHz(3)
void x68k_crtc_set_display_width(uint8_t v);

// Text screen access mask
void x68k_crtc_set_text_screen_access_mask(uint8_t v);

// Text screen simultaneous access
void x68k_crtc_set_text_screen_access(uint8_t v);

// Text screen simultaneous access plane selection AP3
void x68k_crtc_set_text_screen_access_ap3(uint8_t v);

// Text screen simultaneous access plane selection AP2
void x68k_crtc_set_text_screen_access_ap2(uint8_t v);

// Text screen simultaneous access plane selection AP1
void x68k_crtc_set_text_screen_access_ap1(uint8_t v);

// Text screen simultaneous access plane selection AP0
void x68k_crtc_set_text_screen_access_ap0(uint8_t v);

// Text layer raster copy target plane / gvram clear target page
void x68k_crtc_set_cp3(uint8_t v);

// Text layer raster copy target plane / gvram clear target page
void x68k_crtc_set_cp2(uint8_t v);

// Text layer raster copy target plane / gvram clear target page
void x68k_crtc_set_cp1(uint8_t v);

// Text layer raster copy target plane / gvram clear target page
void x68k_crtc_set_cp0(uint8_t v);

// Set source raster
void x68k_crtc_set_copy_source(uint8_t v);

// Set destination raster
void x68k_crtc_set_copy_destination(uint8_t v);

// Set text simultaneous mask pattern
void x68k_crtc_set_text_mask_pattern(uint16_t v);

// CRTC control port
void x68k_crtc_set_control(uint8_t v);

#endif // CRTC_H
