#ifndef _XT_H
#define _XT_H

#include <stdint.h>
#include "common.h"
#include "x68000/x68k_opm.h"
#include "xt_track.h"
#include "xt_mod.h"

// Status of a single FM channel's pitch, which we must track to support
// simple portamento effects and vibrato.
typedef struct XtFmPitch
{
	uint8_t octave;
	X68kOpmNote note;
	uint8_t fraction;
} XtFmPitch;

typedef enum XtFmKeyState
{
	KEY_STATE_OFF,
	KEY_STATE_ON,
	KEY_STATE_ON_PENDING,
	KEY_STATE_CUT,
} XtFmKeyState;

typedef struct XtFmChannelState
{
	// FM registers are compared against the last written values, so as to
	// avoid unnecessary writes (which are slow).
	// Octave and note are ignored.
	XtInstrument instrument;
	XtInstrument instrument_prev;

	XtMod mod_vibrato;
	XtMod mod_tremolo;

	XtFmPitch current_pitch;  // Set at the time a note is played.
	XtFmPitch target_pitch;  // This is what is sent to the register.

	// Pitch patch information calculated after processing pitch.
	uint8_t reg_28_cache;
	uint8_t reg_30_cache;
	uint8_t reg_28_cache_prev;
	uint8_t reg_30_cache_prev;

	uint16_t portamento_speed;
	uint16_t amplitude;

	XtFmKeyState key_state;
	XtFmKeyState key_state_prev;
	uint16_t key_on_delay_count;  // Decrements when nonzero on tick.
	uint16_t mute_delay_count;
	uint16_t cut_delay_count;

	uint8_t reg_20_overlay;
	int8_t tune;  // Fine offset to be applied to pitch fraction.

} XtFmChannelState;

typedef struct Xt
{
	XtTrack track;
	XtFmChannelState fm_state[XT_FM_CHANNEL_COUNT];

	uint16_t current_frame;  // Index into the entire track.
	uint16_t current_phrase_row;  // Index into the current phrase.

	uint16_t ticks_per_row;  // Ticks per row.
	uint16_t tick_counter;  // Counts down from the period.
	uint16_t timer_period;  // Period of timer ticks.

	uint16_t noise_enable;
} Xt;

void xt_tick(Xt *xt);
void xt_update_opm_registers(Xt *xt);

#endif  // _XT_H
