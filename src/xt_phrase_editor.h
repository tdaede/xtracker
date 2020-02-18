#ifndef _XT_PHRASE_EDITOR_H
#define _XT_PHRASE_EDITOR_H

#include "xt_track.h"
#include "xt_keys.h"
#include "xt_render.h"
#include <stdint.h>

typedef enum XtEditorCursorSubPos
{
	CURSOR_SUBPOS_NOTE,
	CURSOR_SUBPOS_INSTRUMENT_HIGH,
	CURSOR_SUBPOS_INSTRUMENT_LOW,
	CURSOR_SUBPOS_CMD1,
	CURSOR_SUBPOS_ARG1_HIGH,
	CURSOR_SUBPOS_ARG1_LOW,
	CURSOR_SUBPOS_CMD2,
	CURSOR_SUBPOS_ARG2_HIGH,
	CURSOR_SUBPOS_ARG2_LOW,
	CURSOR_SUBPOS_MAX_INVALID,

	CURSOR_SUBPOS_INVALID = 0xFFFF,
} XtEditorCursorSubPos;

typedef enum XtEditorState
{
	EDITOR_NORMAL,
	EDITOR_SELECTING,
} XtEditorState;

typedef struct XtPhraseEditor
{
	XtEditorState state;
	XtEditorCursorSubPos sub_pos;  // What is being edited within the column.
	uint16_t frame;  // Which frame is being edited.
	uint16_t row;  // Index into the Frame.
	uint16_t column;  // Horizontally, which column the cursor lies in.
	uint16_t yscroll;  // PCG BG plane Y-scroll to be used during editing.

	uint16_t instrument;  // Currently selected instrument number.
	uint16_t octave;  // Currently selected entry octave (for the bottom row).
	uint16_t step_size;  // Rows to go down after having entered a note.

	uint16_t channel_dirty[XT_TOTAL_CHANNEL_COUNT];

	// TODO: Clipboard buffer, and all that...

} XtPhraseEditor;

void xt_phrase_editor_init(XtPhraseEditor *p);
void xt_phrase_editor_tick(XtPhraseEditor *p, XtTrack *t, const XtKeys *k);
void xt_phrase_editor_render(const XtPhraseEditor *p);
void xt_phrase_editor_update_renderer(XtPhraseEditor *p, XtTrackRenderer *r);

#endif  // _XT_PHRASE_EDITOR_H
