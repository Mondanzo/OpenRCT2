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
#include "../../drawing/DrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../MouseEventArgs.h"
#include "../Window.h"
#include "Button.h"

using namespace OpenRCT2::Ui;

constexpr sint32 CLICK_REPEAT_INITIAL_TIMEOUT = 30;
constexpr sint32 CLICK_REPEAT_DELAY = 4;

void Button::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        if (Type == BUTTON_TYPE::FLAT)
        {
            Width = 24;
            Height = 24;
        }
        else if (Type == BUTTON_TYPE::OUTSET)
        {
            Height = 12;
        }
    }
}

void Button::Update()
{
    bool isCursor = ((Flags & WIDGET_FLAGS::CURSOR) != 0);
    if (isCursor != IsHighlighted())
    {
        if (isCursor)
        {
            ButtonFlags |= BUTTON_FLAGS::HIGHLIGHTED;
        }
        else
        {
            ButtonFlags &= ~BUTTON_FLAGS::HIGHLIGHTED;
        }
        InvalidateVisual();
    }

    if ((ButtonFlags & BUTTON_FLAGS::CLICK_REPEAT) &&
        IsHighlighted() &&
        IsPressed())
    {
        _clickRepeatTimeout--;
        if (_clickRepeatTimeout <= 0)
        {
            _clickRepeatTimeout = CLICK_REPEAT_DELAY;
            auto handler = ClickEvent;
            if (handler != nullptr)
            {
                handler();
            }
        }
    }
}

void Button::Draw(IDrawingContext * dc)
{
    switch (Type) {
    case BUTTON_TYPE::FLAT:
        DrawFlat(dc);
        break;
    case BUTTON_TYPE::OUTSET:
        DrawOutset(dc);
        break;
    case BUTTON_TYPE::IMAGE:
        DrawImage(dc);
        break;
    }
}

void Button::DrawFlat(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);

    // Border
    if (!IsDisabled() && (IsHighlighted() || IsPressed()))
    {
        uint8 rectFlags = 0;
        if (IsHighlighted() && IsPressed())
        {
            rectFlags = INSET_RECT_FLAG_BORDER_INSET;
            if (Image == (uint32)-2)
            {
                rectFlags = INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_NONE;
            }
        }
        dc->FillRect3D(0, 0, Width - 1, Height - 1, colour, rectFlags);
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

void Button::DrawOutset(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);

    // Border
    uint8 rectFlags = 0;
    if (ButtonFlags & BUTTON_FLAGS::STYLE_LIGHT)
    {
        rectFlags |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    }
    if (IsHighlighted() && IsPressed())
    {
        rectFlags |= INSET_RECT_FLAG_BORDER_INSET;
    }
    dc->FillRect3D(0, 0, Width - 1, Height - 1, colour, rectFlags);

    // Text
    if (Text != STR_NONE)
    {
        sint32 l = (Width / 2) - 1;
        sint32 t = Math::Max(0, (Height / 2) - 6);
        if (IsDisabled())
        {
            colour |= COLOUR_FLAG_INSET;
        }

        uint32 stringFlags = STRING_FLAGS::HALIGN_MIDDLE |
                             STRING_FLAGS::CLIPPED;
        dc->DrawString(Text, nullptr, l, t, colour, stringFlags, Width - 3);
    }
}

void Button::DrawImage(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);
    uint32 sprite = Image;
    if ((IsHighlighted() && IsPressed()) || IsDown())
    {
        sprite = ImageDown;
    }
    sprite |= 0x20000000;
    sprite |= colour << 19;
    dc->DrawSprite(sprite, 0, 0, 0);
}

void Button::MouseDown(const MouseEventArgs * e)
{
    if (e->Button == MOUSE_BUTTON::LEFT)
    {
        ButtonFlags |= BUTTON_FLAGS::PRESSED;
        InvalidateVisual();

        if (ButtonFlags & BUTTON_FLAGS::CLICK_REPEAT)
        {
            _clickRepeatTimeout = CLICK_REPEAT_INITIAL_TIMEOUT;
            InvokeClick();
        }
    }
}

void Button::MouseUp(const MouseEventArgs * e)
{
    if (e->Button == MOUSE_BUTTON::LEFT)
    {
        ButtonFlags &= ~BUTTON_FLAGS::PRESSED;
        InvalidateVisual();

        if ((ButtonFlags & BUTTON_FLAGS::HIGHLIGHTED) &&
            !(ButtonFlags & BUTTON_FLAGS::CLICK_REPEAT))
        {
            InvokeClick();
        }
    }
}

void Button::InvokeClick()
{
    auto handler = ClickEvent;
    if (handler != nullptr)
    {
        handler();
    }
}

bool Button::IsHighlighted()
{
    bool isHighlighted = ((ButtonFlags & BUTTON_FLAGS::HIGHLIGHTED) != 0);
    return isHighlighted;
}

bool Button::IsPressed()
{
    bool isPressed = (ButtonFlags & BUTTON_FLAGS::PRESSED) != 0;
    return isPressed;
}

bool Button::IsDown()
{
    bool isDown = (ButtonFlags & BUTTON_FLAGS::DOWN) != 0;
    return isDown;
}
