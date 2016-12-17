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
#include "Button.h"
#include "Spinner.h"

extern "C"
{
    #include "../../interface/colour.h"
    #include "../../localisation/localisation.h"
}

using namespace OpenRCT2::Ui;

Spinner::Spinner()
{
    _upButton = new Button();
    _upButton->Type = BUTTON_TYPE::OUTSET;
    _upButton->Flags &= ~WIDGET_FLAGS::AUTO_SIZE;
    _upButton->ButtonFlags |= BUTTON_FLAGS::CLICK_REPEAT;
    _upButton->Text = STR_NUMERIC_UP;
    _upButton->ClickEvent = std::bind(&Spinner::UpHandler, this);

    _downButton = new Button();
    _downButton->Type = BUTTON_TYPE::OUTSET;
    _downButton->Flags &= ~WIDGET_FLAGS::AUTO_SIZE;
    _downButton->ButtonFlags |= BUTTON_FLAGS::CLICK_REPEAT;
    _downButton->Text = STR_NUMERIC_DOWN;
    _downButton->ClickEvent = std::bind(&Spinner::DownHandler, this);
}

Spinner::~Spinner()
{
    delete _upButton;
    delete _downButton;
}

sint32 Spinner::GetChildrenCount()
{
    return 2;
}

Widget * Spinner::GetChild(sint32 index)
{
    switch (index) {
    case 0: return _upButton;
    case 1: return _downButton;
    default: return nullptr;
    }
}

void Spinner::Measure()
{
    if (Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        Height = 12;
    }
}

void Spinner::Arrange()
{
    sint32 buttonWidth = 11;
    sint32 buttonHeight = (Height - 2) / 2;
    sint32 buttonX = Width - 11;

    _upButton->X = buttonX;
    _upButton->Y = 1;
    _upButton->Width = buttonWidth;
    _upButton->Height = buttonHeight;

    _downButton->X = buttonX;
    _downButton->Y = Height - 1 - buttonHeight;
    _downButton->Width = buttonWidth;
    _downButton->Height = buttonHeight;
}

void Spinner::Draw(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);

    uint8 press = INSET_RECT_F_60;
    DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, colour, press);

    // Draw text
    rct_string_id stringId = STR_CURRENCY_FORMAT_LABEL;
    if (SpinnerFlags & SPINNER_FLAGS::HIGH_PRECISION)
    {
        stringId = STR_BOTTOM_TOOLBAR_CASH;
    }
    if (Value == 0 && (SpinnerFlags & SPINNER_FLAGS::SHOW_ZERO_AS_FREE))
    {
        stringId = STR_FREE;
    }

    sint32 l = 1;
    sint32 t = 1;
    uintptr_t dpip = ((uintptr_t *)dc)[2];
    rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
    gfx_draw_string_left(dpi, stringId, &Value, colour, l, t);
}

void Spinner::UpHandler()
{
    if (IncrementEvent != nullptr)
    {
        IncrementEvent();
    }
}

void Spinner::DownHandler()
{
    if (DecrementEvent != nullptr)
    {
        DecrementEvent();
    }
}
