#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "audio/audio.h"
#include "audio/AudioMixer.h"
#include "Context.h"
#include "drawing/drawing.h"
#include "interface/widget.h"
#include "interface/window.h"
#include "intro.h"
#include "sprites.h"

#define BACKROUND_COLOUR_DARK       PALETTE_INDEX_10
#define BACKROUND_COLOUR_LOGO       PALETTE_INDEX_245
#define BORDER_COLOUR_PUBLISHER     PALETTE_INDEX_129

#define PALETTE_G1_IDX_DEVELOPER    23217
#define PALETTE_G1_IDX_LOGO         23224

enum INTRO_STATE {
    INTRO_STATE_NONE,
    INTRO_STATE_PUBLISHER_BEGIN,
    INTRO_STATE_PUBLISHER_SCROLL,
    INTRO_STATE_DEVELOPER_BEGIN,
    INTRO_STATE_DEVELOPER_SCROLL,
    INTRO_STATE_LOGO_FADE_IN,
    INTRO_STATE_LOGO_WAIT,
    INTRO_STATE_LOGO_FADE_OUT,
    INTRO_STATE_DISCLAIMER_1,
    INTRO_STATE_DISCLAIMER_2,
    INTRO_STATE_CLEAR = 254,
    INTRO_STATE_FINISH = 255,
};

// Used mainly for timing but also for Y coordinate and fading.
static uint8 _introState;
static sint32 _introStateCounter;

static void *_soundChannel = NULL;
static bool _chainLiftFinished;

static void screen_intro_process_mouse_input();
static void screen_intro_process_keyboard_input();
static void screen_intro_skip_part();
static void screen_intro_draw_logo(rct_drawpixelinfo *dpi);

static void window_intro_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list _introEvents =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_intro_paint,
    NULL
};

static rct_widget _introWidgets[] =
{
    { WIDGETS_END }
};

bool intro_is_finished()
{
    return _introState == INTRO_STATE_FINISH;
}

void intro_begin()
{
    _introState = INTRO_STATE_PUBLISHER_BEGIN;

    rct_window * w = window_create(0, 0, context_get_width(), context_get_height(), &_introEvents, WC_INTRO, WF_STICK_TO_BACK);
    w->widgets = _introWidgets;
}

// rct2: 0x0068E966
void intro_update()
{
    screen_intro_process_mouse_input();
    screen_intro_process_keyboard_input();

    switch (_introState) {
    case INTRO_STATE_DISCLAIMER_1:
    case INTRO_STATE_DISCLAIMER_2:
        // Originally used for the disclaimer text
        _introState = INTRO_STATE_PUBLISHER_BEGIN;
    case INTRO_STATE_PUBLISHER_BEGIN:
        load_palette();

        // Set the Y for the Infogrames logo
        _introStateCounter = -580;

        // Play the chain lift sound
        _soundChannel = Mixer_Play_Effect(SOUND_LIFT_7, MIXER_LOOP_INFINITE, MIXER_VOLUME_MAX, 0.5f, 1, true);
        _chainLiftFinished = false;
        _introState++;
        break;
    case INTRO_STATE_PUBLISHER_SCROLL:
        // Move the Infogrames logo down
        _introStateCounter += 5;

        // Check if logo is off the screen...ish
        if (_introStateCounter > context_get_height() - 120) {
            _introStateCounter = -116;
            _introState++;
        }

        break;
    case INTRO_STATE_DEVELOPER_BEGIN:
        // Set the Y for the Chris Sawyer logo
        _introStateCounter = -116;

        _introState++;
        break;
    case INTRO_STATE_DEVELOPER_SCROLL:
        _introStateCounter += 5;

        // Check if logo is almost scrolled to the bottom
        if (!_chainLiftFinished && _introStateCounter >= context_get_height() + 40 - 421) {
            _chainLiftFinished = true;

            // Stop the chain lift sound
            if (_soundChannel != NULL) {
                Mixer_Stop_Channel(_soundChannel);
                _soundChannel = NULL;
            }

            // Play the track friction sound
            _soundChannel = Mixer_Play_Effect(SOUND_TRACK_FRICTION_3, MIXER_LOOP_INFINITE, MIXER_VOLUME_MAX, 0.25f, 0.75, true);
        }

        // Check if logo is off the screen...ish
        if (_introStateCounter >= context_get_height() + 40) {
            // Stop the track friction sound
            if (_soundChannel != NULL) {
                Mixer_Stop_Channel(_soundChannel);
                _soundChannel = NULL;
            }

            // Play long peep scream sound
            _soundChannel = Mixer_Play_Effect(SOUND_SCREAM_1, MIXER_LOOP_NONE, MIXER_VOLUME_MAX, 0.5f, 1, false);

            _introState++;
            _introStateCounter = 0;
        }
        break;
    case INTRO_STATE_LOGO_FADE_IN:
        // Fade in, add 4 / 256 to fading
        _introStateCounter += 0x400;
        if (_introStateCounter > 0xFF00) {
            _introState++;
            _introStateCounter = 0;
        }
        break;
    case INTRO_STATE_LOGO_WAIT:
        // Wait 80 game ticks
        _introStateCounter++;
        if (_introStateCounter >= 80) {
            // Set fading to 256
            _introStateCounter = 0xFF00;

            _introState++;
        }
        break;
    case INTRO_STATE_LOGO_FADE_OUT:
        // Fade out, subtract 4 / 256 from fading
        _introStateCounter -= 0x400;
        if (_introStateCounter < 0) {
            _introState = INTRO_STATE_CLEAR;
        }
        break;
    case INTRO_STATE_CLEAR:
        // Stop any playing sound
        if (_soundChannel != NULL) {
            Mixer_Stop_Channel(_soundChannel);
            _soundChannel = NULL;
        }

        // Move to next part
        _introState++;
        _introStateCounter = 0;
        break;
    case INTRO_STATE_FINISH:
        break;
    }

    rct_window * w = window_find_by_class(WC_INTRO);
    if (w != NULL)
    {
        w->width = context_get_width();
        w->height = context_get_height();
        window_invalidate(w);
    }
}

static void window_intro_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
    sint32 screenWidth = context_get_width();

    switch (_introState) {
    case INTRO_STATE_DISCLAIMER_1:
    case INTRO_STATE_DISCLAIMER_2:
        break;
    case INTRO_STATE_PUBLISHER_BEGIN:
        gfx_clear(dpi, BACKROUND_COLOUR_DARK);
        break;
    case INTRO_STATE_PUBLISHER_SCROLL:
        gfx_clear(dpi, BACKROUND_COLOUR_DARK);

        // Draw a white rectangle for the logo background (gives a bit of white margin)
        gfx_fill_rect(dpi,
            (screenWidth / 2) - 320 + 50, _introStateCounter + 50,
            (screenWidth / 2) - 320 + 50 + 540, _introStateCounter + 50 + 425,
            BORDER_COLOUR_PUBLISHER);

        // Draw Infogrames logo
        gfx_draw_sprite(dpi, SPR_INTRO_INFOGRAMES_00, (screenWidth / 2) - 320 + 69, _introStateCounter + 69, 0);
        gfx_draw_sprite(dpi, SPR_INTRO_INFOGRAMES_10, (screenWidth / 2) - 320 + 319, _introStateCounter + 69, 0);
        gfx_draw_sprite(dpi, SPR_INTRO_INFOGRAMES_01, (screenWidth / 2) - 320 + 69, _introStateCounter + 319, 0);
        gfx_draw_sprite(dpi, SPR_INTRO_INFOGRAMES_11, (screenWidth / 2) - 320 + 319, _introStateCounter + 319, 0);
        break;
    case INTRO_STATE_DEVELOPER_BEGIN:
        gfx_clear(dpi, BACKROUND_COLOUR_DARK);
        gfx_transpose_palette(PALETTE_G1_IDX_DEVELOPER, 255);
        break;
    case INTRO_STATE_DEVELOPER_SCROLL:
        gfx_clear(dpi, BACKROUND_COLOUR_DARK);

        // Draw Chris Sawyer logo
        gfx_draw_sprite(dpi, SPR_INTRO_CHRIS_SAWYER_00, (screenWidth / 2) - 320 + 70, _introStateCounter, 0);
        gfx_draw_sprite(dpi, SPR_INTRO_CHRIS_SAWYER_10, (screenWidth / 2) - 320 + 320, _introStateCounter, 0);
        break;
    case INTRO_STATE_LOGO_FADE_IN:
        if (_introStateCounter <= 0xFF00) {
            gfx_transpose_palette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
        } else {
            gfx_transpose_palette(PALETTE_G1_IDX_LOGO, 255);
        }
        screen_intro_draw_logo(dpi);
        break;
    case INTRO_STATE_LOGO_WAIT:
        screen_intro_draw_logo(dpi);
        break;
    case INTRO_STATE_LOGO_FADE_OUT:
        if (_introStateCounter >= 0) {
            gfx_transpose_palette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
        } else {
            gfx_transpose_palette(PALETTE_G1_IDX_LOGO, 0);
        }
        screen_intro_draw_logo(dpi);
        break;
    case INTRO_STATE_CLEAR:
        gfx_clear(dpi, BACKROUND_COLOUR_DARK);
        break;
    }
}

static void screen_intro_process_mouse_input()
{
    if (context_get_cursor_state()->any == CURSOR_PRESSED) {
        screen_intro_skip_part();
    }
}

/**
 *
 *  rct2: 0x006E3AEC
 */
static void screen_intro_process_keyboard_input()
{
    const uint8 * keys = context_get_keys_state();
    for (int i = 0; i < 256; i++) {
        if (keys[i] != 0) {
            screen_intro_skip_part();
            break;
        }
    }
}

static void screen_intro_skip_part()
{
    switch (_introState) {
    case INTRO_STATE_NONE:
        break;
    case INTRO_STATE_DISCLAIMER_2:
        _introState = INTRO_STATE_PUBLISHER_BEGIN;
        break;
    default:
        _introState = INTRO_STATE_CLEAR;
        break;
    }
}

static void screen_intro_draw_logo(rct_drawpixelinfo *dpi)
{
    sint32 screenWidth = context_get_width();
    sint32 imageWidth = 640;
    sint32 imageX = (screenWidth - imageWidth) / 2;

    drawing_engine_invalidate_image(SPR_INTRO_LOGO_00);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_10);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_20);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_01);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_11);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_21);

    gfx_clear(dpi, BACKROUND_COLOUR_LOGO);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_00, imageX + 0,     0, 0);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_10, imageX + 220,   0, 0);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_20, imageX + 440,   0, 0);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_01, imageX + 0,   240, 0);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_11, imageX + 220, 240, 0);
    gfx_draw_sprite(dpi, SPR_INTRO_LOGO_21, imageX + 440, 240, 0);
}
