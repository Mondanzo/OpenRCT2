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
#include "../Window.h"
#include "Chart.h"

extern "C"
{
    #include "../../interface/graph.h"
}

using namespace OpenRCT2::Ui;

void Chart::SetValues(const uint8 * values, size_t count)
{
    _values = values;
    _valuesCount = count;
}

void Chart::Draw(IDrawingContext * dc)
{
    uintptr_t dpip = ((uintptr_t *)dc)[2];
    rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;

    const WindowStyle * style = GetParentWindow()->GetStyle();
    colour_t colour = style->GetColour(GetStyle());
    dc->FillRect3D(0, 0, Width - 1, Height - 1, colour, INSET_RECT_F_30);

    if (_values != nullptr && _valuesCount > 0)
    {
        sint32 x = 18;
        sint32 y = 11;
        graph_draw_uint8(dpi, (uint8 *)_values, (int)_valuesCount, x, y);
    }
}
