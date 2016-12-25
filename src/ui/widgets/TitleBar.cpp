#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../../drawing/DrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../DrawingContextExtensions.h"
#include "../MouseEventArgs.h"
#include "../Window.h"
#include "TitleBar.h"

extern "C"
{
    #include "../../interface/colour.h"
    #include "../../localisation/localisation.h"
}

using namespace OpenRCT2::Ui;

std::string TitleBar::GetText()
{
    return _text;
}

void TitleBar::SetText(const std::string &value)
{
    _text = value;
}

void TitleBar::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        Height = 14;
    }
}

void TitleBar::Draw(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);

    uint8 press = INSET_RECT_F_60;
    press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    // if (w->flags & WF_10)
    // {
    //     press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    // }
    DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, colour, press);

    // Black caption bars look slightly green, this fixes that
    if (colour == 0)
    {
        dc->FillRect(1, 1, Width - 2, Height - 2, ColourMapA[colour].dark);
    }
    else
    {
        dc->FilterRect(PALETTE_DARKEN_3, 1, 1, Width - 2, Height - 2);
    }

    // Draw text
    if (!_text.empty())
    {
        const char * text = _text.c_str();
        sint32 l = 2;
        sint32 t = 1;
        sint32 width = Width - 4;
        l += width / 2;

        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        gfx_draw_string_centred_clipped(dpi, STR_STRING, &text, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, l, t, width);
    }
}

void TitleBar::MouseDown(const MouseEventArgs * e)
{
    if (e->Button == MOUSE_BUTTON::LEFT)
    {
        _movingWindow = true;
        _lastCursorPosition = { e->X, e->Y };
    }
}

void TitleBar::MouseMove(const MouseEventArgs * e)
{
    if (_movingWindow)
    {
        xy32 lastPos = _lastCursorPosition;
        xy32 offset = { e->X - lastPos.X, e->Y - lastPos.Y };
        if (offset.X != 0 || offset.Y != 0)
        {
            Window * window = ParentWindow;
            xy32 location = window->Location;
            location.X += offset.X;
            location.Y += offset.Y;
            window->SetLocation(location);
        }
        _lastCursorPosition = { e->X - offset.X, e->Y - offset.Y };
    }
}

void TitleBar::MouseUp(const MouseEventArgs * e)
{
    if (e->Button == MOUSE_BUTTON::LEFT)
    {
        _movingWindow = false;
    }
}
