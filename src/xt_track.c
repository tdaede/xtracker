#include "xt_track.h"

uint16_t xt_track_get_phrase_number_for_frame(XtTrack *t, uint16_t channel,
                                              uint16_t frame)
{
	const uint16_t idx_base = (XT_PHRASES_PER_CHANNEL * channel);
	const uint16_t idx = t->frames[frame].row_idx[channel];
	return idx_base + idx;
}
