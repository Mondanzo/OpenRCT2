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
#include "../WindowManager.h"
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
        IWindowManager * wm = GetWindowManager();
        sint32 lineHeight = wm->GetLineHeight(FONT_SIZE_MEDIUM);

        SetHeight(lineHeight + 2);
        if (_wrap)
        {
            _lastMeasuredWidth = GetWidth();

            char buffer[4096];
            String::Set(buffer, sizeof(buffer), _text.c_str());

            sint32 numLines;
            sint32 fontSpriteBase;
            gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            gfx_wrap_string(buffer, GetWidth(), &numLines, &fontSpriteBase);
            numLines++;
            sint32 lineHeight = font_get_line_height(fontSpriteBase);
            SetHeight(numLines * lineHeight + 2);
        }
    }
}

void TextBlock::Update()
{
    if (_wrap && _lastMeasuredWidth != GetWidth())
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
        sint32 width = GetWidth() - 4;
        uint32 stringFlags = 0;

        if (HorizontalAlignment == HORIZONTAL_ALIGNMENT::MIDDLE)
        {
            l = 2 + (width / 2);
            stringFlags |= STRING_FLAGS::HALIGN_MIDDLE;
        }
        if (_wrap)
        {
            stringFlags |= STRING_FLAGS::WRAPPED;
        }

        dc->DrawString(text, l, t, _colour, stringFlags, width);
    }
}
