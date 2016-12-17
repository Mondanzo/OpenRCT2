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

#include "../../drawing/IDrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../DrawingContextExtensions.h"
#include "../Window.h"
#include "TitleBar.h"

extern "C"
{
    #include "../../interface/colour.h"
    #include "../../localisation/localisation.h"
}

using namespace OpenRCT2::Ui;

void TitleBar::Draw(IDrawingContext * dc)
{
    colour_t colour = Window->Style.GetColour(Style);

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
    if (Text != STR_NONE)
    {
        sint32 l = 2;
        sint32 t = 1;
        sint32 width = Width - 4;
        l += width / 2;

        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        gfx_draw_string_centred_clipped(dpi, Text, nullptr, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, l, t, width);
    }
}
