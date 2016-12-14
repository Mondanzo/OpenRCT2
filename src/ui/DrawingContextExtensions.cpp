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

#include "../drawing/IDrawingContext.h"
#include "DrawingContextExtensions.h"

extern "C"
{
    #include "../interface/colour.h"
}

namespace DCExtensions
{
    void FillRectInset(IDrawingContext * dc, sint32 left, sint32 top, sint32 right, sint32 bottom, uint32 colour, uint32 flags)
    {
        uint8 shadow, fill, hilight;

        if (colour & (COLOUR_FLAG_TRANSLUCENT | COLOUR_FLAG_8))
        {
            translucent_window_palette palette;
            if (colour & COLOUR_FLAG_8)
            {
                // TODO: This can't be added up
                // palette = NOT_TRANSLUCENT(colour);
                assert(false);
                return;
            }
            else
            {
                palette = TranslucentWindowPalettes[BASE_COLOUR(colour)];
            }

            if (flags & INSET_RECT_FLAG_BORDER_NONE)
            {
                dc->FilterRect(palette.base, left, top, right, bottom);
            }
            else if (flags & INSET_RECT_FLAG_BORDER_INSET)
            {
                // Draw outline of box
                dc->FilterRect(palette.highlight, left, top, left, bottom);
                dc->FilterRect(palette.highlight, left, top, right, top);
                dc->FilterRect(palette.shadow, right, top, right, bottom);
                dc->FilterRect(palette.shadow, left, bottom, right, bottom);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    dc->FilterRect(palette.base, left + 1, top + 1, right - 1, bottom - 1);
                }
            }
            else
            {
                // Draw outline of box
                dc->FilterRect(palette.shadow, left, top, left, bottom);
                dc->FilterRect(palette.shadow, left, top, right, top);
                dc->FilterRect(palette.highlight, right, top, right, bottom);
                dc->FilterRect(palette.highlight, left, bottom, right, bottom);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    dc->FilterRect(palette.base, left + 1, top + 1, right - 1, bottom - 1);
                }
            }
        }
        else
        {
            if (flags & INSET_RECT_FLAG_FILL_MID_LIGHT)
            {
                shadow = ColourMapA[colour].dark;
                fill = ColourMapA[colour].mid_light;
                hilight = ColourMapA[colour].lighter;
            }
            else
            {
                shadow = ColourMapA[colour].mid_dark;
                fill = ColourMapA[colour].light;
                hilight = ColourMapA[colour].lighter;
            }

            if (flags & INSET_RECT_FLAG_BORDER_NONE)
            {
                dc->FillRect(fill, left, top, right, bottom);
            }
            else if (flags & INSET_RECT_FLAG_BORDER_INSET)
            {
                // Draw outline of box
                dc->FillRect(shadow, left, top, left, bottom);
                dc->FillRect(shadow, left + 1, top, right, top);
                dc->FillRect(hilight, right, top + 1, right, bottom - 1);
                dc->FillRect(hilight, left + 1, bottom, right, bottom);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    if (!(flags & INSET_RECT_FLAG_FILL_DONT_LIGHTEN))
                    {
                        if (flags & INSET_RECT_FLAG_FILL_GREY)
                        {
                            fill = ColourMapA[COLOUR_BLACK].light;
                        }
                        else
                        {
                            fill = ColourMapA[colour].lighter;
                        }
                    }
                    dc->FillRect(fill, left + 1, top + 1, right - 1, bottom - 1);
                }
            }
            else
            {
                // Draw outline of box
                dc->FillRect(hilight, left, top, left, bottom - 1);
                dc->FillRect(hilight, left + 1, top, right - 1, top);
                dc->FillRect(shadow, right, top, right, bottom - 1);
                dc->FillRect(shadow, left, bottom, right, bottom);

                if (!(flags & INSET_RECT_FLAG_FILL_NONE))
                {
                    if (flags & INSET_RECT_FLAG_FILL_GREY)
                    {
                        fill = ColourMapA[COLOUR_BLACK].light;
                    }
                    dc->FillRect(fill, left + 1, top + 1, right - 1, bottom - 1);
                }
            }
        }
    }
}
