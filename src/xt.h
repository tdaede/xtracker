#ifndef _XT_H
#define _XT_H

#include <stdint.h>
#include "common.h"
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

typedef enum XtFmKeyCommand
{
	KEY_COMMAND_NONE,
	KEY_COMMAND_ON,
	KEY_COMMAND_OFF,
} XtFmKeyCommand;

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

	XtFmKeyCommand key_command;

	uint16_t cache_invalid;
} XtFmChannelState;

typedef struct Xt
{
	XtTrack track;
	XtFmChannelState fm_state[XT_FM_CHANNEL_COUNT];

	uint16_t current_frame;  // Index into the entire track.
	uint16_t current_phrase_row;  // Index into the current phrase.

	uint16_t current_ticks_per_row;  // Ticks per row.
	uint16_t tick_counter;  // Counts down from the period.
	uint16_t timer_period;  // Period of timer ticks.

	uint16_t noise_enable;

	uint16_t playing;
	uint16_t repeat_frame;
} Xt;

void xt_init(Xt *xt);
void xt_tick(Xt *xt);
void xt_update_opm_registers(Xt *xt);

// -1 to resume playback at current frame
// repeat to cause it to play the same frame repeatedly
void xt_start_playing(Xt *xt, int16_t frame, uint16_t repeat);
void xt_stop_playing(Xt *xt);

#endif  // _XT_H
