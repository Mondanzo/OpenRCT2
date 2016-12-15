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
#include "Button.h"

using namespace OpenRCT2::Ui;

void Button::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        if (Style == BUTTON_STYLE::FLAT)
        {
            Width = 24;
            Height = 24;
        }
    }
}

void Button::Draw(IDrawingContext * dc)
{
    if (Style == BUTTON_STYLE::FLAT)
    {
        bool isHighlighted = (_buttonFlags & BUTTON_FLAGS::HIGHLIGHTED) != 0;
        if (!IsDisabled() && isHighlighted)
        {
            // widget_button_draw(dpi, w, widgetIndex);
            return;
        }

        // Get the colour
        // uint8 colour = w->colours[widget->colour];
        uint8 colour = 0;

        // Check if the button is pressed down
        bool isPressed = (_buttonFlags & BUTTON_FLAGS::PRESSED) != 0;
        if (isPressed)
        {
            if (Image == (uint32)-2)
            {
                // Draw border with no fill
                // gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE);
                return;
            }

            // Draw the border with fill
            // gfx_fill_rect_inset(dpi, l, t, r, b, colour, INSET_RECT_FLAG_BORDER_INSET);
        }

        // Draw image
        if (IsDisabled())
        {
            // Draw greyed out (light border bottom right shadow)
            colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
            dc->DrawSpriteSolid(Image, 1, 1, colour);

            // Draw greyed out (dark)
            colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
            dc->DrawSpriteSolid(Image, 0, 0, colour);
        }
        else
        {
            uint32 sprite = Image;
            if (sprite & 0x40000000)
            {
                sprite &= ~0x40000000;
            }
            else
            {
                sprite |= colour << 19;
            }
            dc->DrawSprite(sprite, 0, 0, 0);
        }
    }
    else
    {
        // Check if the button is pressed down
        uint8 press = 0;
        // if (w->flags & WF_10)
            press |= INSET_RECT_FLAG_FILL_MID_LIGHT;

        bool isPressed = (_buttonFlags & BUTTON_FLAGS::PRESSED) != 0;
        if (isPressed)
        {
            press |= INSET_RECT_FLAG_BORDER_INSET;
        }

        // Get the colour
        uint8 colour = COLOUR_GREY;

        // Draw the button
        DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, colour, press);

        if (Text != STR_NONE)
        {
            sint32 l = (Width / 2) - 1;
            sint32 t = Math::Max(0, (Height / 2) - 6);
            if (IsDisabled())
            {
                colour |= COLOUR_FLAG_INSET;
            }

            uintptr_t dpip = ((uintptr_t *)dc)[2];
            rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
            gfx_draw_string_centred_clipped(dpi, Text, nullptr, colour, l, t, Width - 3);
        }
    }
}

void Button::MouseDown(const MouseEventArgs * e)
{
    _buttonFlags |= BUTTON_FLAGS::PRESSED;
}

void Button::MouseUp(const MouseEventArgs * e)
{
    _buttonFlags &= ~BUTTON_FLAGS::PRESSED;
}

void Button::MouseEnter(const MouseEventArgs * e)
{
    _buttonFlags |= BUTTON_FLAGS::HIGHLIGHTED;
}

void Button::MouseLeave(const MouseEventArgs * e)
{
    _buttonFlags &= ~BUTTON_FLAGS::HIGHLIGHTED;
}
