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

#include "../../core/Math.hpp"
#include "../../drawing/IDrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../DrawingContextExtensions.h"
#include "TextBlock.h"

using namespace OpenRCT2::Ui;

TextBlock::TextBlock()
{
    Flags |= WIDGET_FLAGS::AUTO_SIZE;
}

void TextBlock::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        Height = 12;
    }
}

void TextBlock::Draw(IDrawingContext * dc)
{
    // Draw text
    if (Text != STR_NONE)
    {
        sint32 l = 2;
        sint32 t = 0;
        sint32 width = Width - 4;
        l += width / 2;

        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        gfx_draw_string_centred_clipped(dpi, Text, TextArgs, COLOUR_BLACK, l, t, width);
    }
}
