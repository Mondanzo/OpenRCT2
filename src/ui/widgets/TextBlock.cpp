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
#include "../../core/String.hpp"
#include "../../drawing/DrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../DrawingContextExtensions.h"
#include "TextBlock.h"

using namespace OpenRCT2::Ui;

TextBlock::TextBlock()
{
    Flags |= WIDGET_FLAGS::AUTO_SIZE |
             WIDGET_FLAGS::STRETCH_H;
    HorizontalAlignment = HORIZONTAL_ALIGNMENT::LEFT;
    VerticalAlignment = VERTICAL_ALIGNMENT::MIDDLE;
}

std::string TextBlock::GetText()
{
    return _text;
}

void TextBlock::SetText(const std::string &value)
{
    if (_text != value)
    {
        _text = value;
        InvalidateVisual();
        if (Flags & WIDGET_FLAGS::AUTO_SIZE)
        {
            InvalidateLayout();
        }
    }
}

colour_t TextBlock::GetColour()
{
    return _colour;
}

void TextBlock::SetColour(colour_t value)
{
    if (_colour != value)
    {
        _colour = value;
        InvalidateVisual();
    }
}

void TextBlock::SetWrapping(bool value)
{
    _wrap = value;
}

void TextBlock::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        Height = 12;
        if (_wrap)
        {
            _lastMeasuredWidth = Width;

            char buffer[4096];
            String::Set(buffer, sizeof(buffer), _text.c_str());

            sint32 numLines;
            sint32 fontSpriteBase;
            gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            gfx_wrap_string(buffer, Width, &numLines, &fontSpriteBase);
            numLines++;
            sint32 lineHeight = font_get_line_height(fontSpriteBase);
            Height = numLines * lineHeight + 2;
        }
    }
}

void TextBlock::Update()
{
    if (_wrap && _lastMeasuredWidth != Width)
    {
        InvalidateLayout();
    }
}

void TextBlock::Draw(IDrawingContext * dc)
{
    // Draw text
    if (!_text.empty())
    {
        const char * text = _text.c_str();

        sint32 l = 0;
        sint32 t = 0;
        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        if (HorizontalAlignment == HORIZONTAL_ALIGNMENT::LEFT)
        {
            if (_wrap)
            {
                gfx_draw_string_left_wrapped(dpi, &text, l, t, Width, STR_STRING, _colour);
            }
            else
            {
                gfx_draw_string_left(dpi, STR_STRING, &text, _colour, l, t);
            }
        }
        else
        {
            sint32 width = Width - 4;
            l = 2 + (width / 2);
            gfx_draw_string_centred_clipped(dpi, STR_STRING, &text, _colour, l, t, width);
        }
    }
}
