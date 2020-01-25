#ifndef _X68K_OPM_H
#define _X68K_OPM_H

#include <stdint.h>

#define OPM_BASE 0xE90000

#define OPM_ADDRESS (volatile uint8_t *)(OPM_BASE + 1)
#define OPM_DATA (volatile uint8_t *)(OPM_BASE + 3)
#define OPM_STATUS (volatile uint8_t *)(OPM_BASE + 3)

#define x68k_opm_status() (*OPM_STATUS)

#define x68k_busy() ((*OPM_STATUS) & 0x80)
#define x68k_timer_a() ((*OPM_STATUS) & 0x01)
#define x68k_timer_b() ((*OPM_STATUS) & 0x02)

#define x68k_opm_write(address, data) do \
{ \
	while(*OPM_STATUS & 0x80) continue; \
	*OPM_ADDRESS = (address); \
	*OPM_DATA = (data); \
} while(0);

typedef enum X68kOpmReg
{
	OPM_REG_TEST_MODE = 0x01,
	OPM_REG_KEY_ON = 0x08,
	OPM_REG_NOISE = 0x0F,
	OPM_REG_CLKA_UPPER = 0x10,
	OPM_REG_CLKA_LOWER = 0x11,
	OPM_REG_CLKB = 0x12,
	OPM_REG_TIMER_FLAGS = 0x14,
	OPM_REG_LFO_FREQ = 0x18,
	OPM_REG_LFO_DEPTH = 0x19,
	OPM_REG_CONTROL = 0x1B,
	// These registers are repeated 8 times each, for CH.A-CH.H.
	OPM_CH_PAN_FL_CON = 0x20,
	OPM_CH_OCT_NOTE = 0x28,
	OPM_CH_KF = 0x30,
	OPM_CH_PMS_AMS = 0x38,
	// These registers are repeated 8 times in an inner set for the channels,
	// and then externally four times for the four operators.
	// For example, 0x40 is CH.A OP1; 0x47 is CH.H OP1. 0x48 is CH.A OP2, etc.
	OPM_CH_DT1_MUL = 0x40,
	OPM_CH_TL = 0x60,
	OPM_CH_KS_AR = 0x80,
	OPM_CH_AME_D1R = 0xA0,
	OPM_DT2_D2R = 0xC0,
	OPM_D1L_RR = 0xE0,

	OPM_REG_MAX = 0xFF
} X68kOpmReg;

static inline void x68k_opm_set_test_mode(uint8_t en)
{
	x68k_opm_write(OPM_REG_TEST_MODE, en ? 0x02 : 0x00);
}

// Channel: 0 - 7
// Key:     0 - F
static inline void x68k_opm_set_key_on(uint8_t channel, uint8_t key)
{
	x68k_opm_write(OPM_REG_KEY_ON, channel | (key << 3));
}

// fnoise(Hz) = 4MHZ / (32 * nfreq)
// Nfreq:   0 - 31
static inline void x68k_opm_set_noise(uint8_t en, uint8_t nfreq)
{
	x68k_opm_write(OPM_REG_NOISE, (en ? 0x80 : 0x00) | nfreq);
}

// Period:  0 - 1023
// Ta(sec) = (64 * (1024 - period)) / 4MHz
static inline void x68k_opm_set_clka_period(uint16_t period)
{
	x68k_opm_write(OPM_REG_CLKA_UPPER, period >> 2);
	x68k_opm_write(OPM_REG_CLKA_LOWER, period & 0x0o3);
}

// Period:  0 - 255
// Ta(sec) = (1024 * (256 - period)) / 4MHz
static inline void x68k_opm_set_clkb_period(uint8_t period)
{
	x68k_opm_write(OPM_REG_CLKB, period);
}

#define X68K_OPM_TIMER_FLAG_CSM       0x80
#define X68K_OPM_TIMER_FLAG_F_RESET_B 0x20
#define X68K_OPM_TIMER_FLAG_F_RESET_A 0x10
#define X68K_OPM_TIMER_FLAG_IRQ_EN_B  0x08
#define X68K_OPM_TIMER_FLAG_IRQ_EN_A  0x04
#define X68K_OPM_TIMER_FLAG_LOAD_B    0x02
#define X68K_OPM_TIMER_FLAG_LOAD_A    0x01

static inline void x68k_opm_set_timer_flags(uint8_t flags)
{
	x68k_opm_write(OPM_REG_TIMER_FLAGS, flags);
}

static inline void x68k_opm_set_lfo_freq(uint8_t freq)
{
	x68k_opm_write(OPM_REG_LFO_FREQ, freq);
}

// Depth:  0 - 127
static inline void x68k_opm_set_lfo_am_depth(uint8_t depth)
{
	x68k_opm_write(OPM_REG_LFO_DEPTH, depth);
}

// Depth:  0 - 127
static inline void x68k_opm_set_lfo_pm_depth(uint8_t depth)
{
	x68k_opm_write(OPM_REG_LFO_DEPTH, 0x80 | depth);
}

typedef enum X68kOpmLfoWave
{
	LFO_WAVE_SAW = 0x00,
	LFO_WAVE_RECTANGLE,
	LFO_WAVE_TRIANGLE,
	LFO_WAVE_RAND,
} X68kOpmLfoWave;

static inline void x68k_opm_set_control(uint8_t ct1_adpcm_8mhz, uint8_t ct2_fdc_ready,
                                        X68kOpmLfoWave lfo_wave)
{
	x68k_opm_write(OPM_REG_CONTROL, (ct1_adpcm_8mhz ? 0x80 : 0x00) |
	                                (ct2_fdc_ready ? 0x40 : 0x00) |
	                                lfo_wave);
}

#endif  // _OPM_H
