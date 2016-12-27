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

money32 Spinner::GetValue()
{
    return _value;
}

void Spinner::SetValue(money32 value)
{
    if (_value != value)
    {
        _value = value;
        InvalidateVisual();
    }
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

void Spinner::Update()
{
    if (SpinnerFlags & SPINNER_FLAGS::READ_ONLY)
    {
        _upButton->SetVisibility(VISIBILITY::HIDDEN);
        _downButton->SetVisibility(VISIBILITY::HIDDEN);
    }
    else
    {
        _upButton->SetVisibility(VISIBILITY::VISIBLE);
        _downButton->SetVisibility(VISIBILITY::VISIBLE);
        _upButton->SetEnabled(IsEnabled());
        _downButton->SetEnabled(IsEnabled());
    }
}

void Spinner::Draw(IDrawingContext * dc)
{
    const WindowStyle * style = ParentWindow->GetStyle();
    colour_t colour = style->GetColour(GetStyle());

    if (!(SpinnerFlags & SPINNER_FLAGS::READ_ONLY))
    {
        uint8 press = INSET_RECT_F_60;
        dc->FillRect3D(0, 0, Width - 1, Height - 1, colour, press);
    }

    // Draw text
    rct_string_id stringId = STR_CURRENCY_FORMAT_LABEL;
    if (SpinnerFlags & SPINNER_FLAGS::HIGH_PRECISION)
    {
        stringId = STR_BOTTOM_TOOLBAR_CASH;
    }
    if (_value == 0 && (SpinnerFlags & SPINNER_FLAGS::SHOW_ZERO_AS_FREE))
    {
        stringId = STR_FREE;
    }
    sint32 l = 1;
    sint32 t = 1;
    dc->DrawString(stringId, &_value, l, t, colour, 0, 0);
}

void Spinner::MouseWheel(const MouseEventArgs * e)
{
    if (IsDisabled() || (SpinnerFlags & SPINNER_FLAGS::READ_ONLY))
    {
        return;
    }

    money32 newValue = _value;
    sint32 deltaWhole = Math::Max(1, abs(e->Delta) / 17);
    sint32 step = SmallStep;
    if (e->Delta < 0)
    {
        while (deltaWhole > 0 && newValue + step <= MaximumValue)
        {
            newValue += step;
            deltaWhole--;
        }
    }
    else if (e->Delta > 0)
    {
        while (deltaWhole > 0 && newValue - step >= MinimumValue)
        {
            newValue -= step;
            deltaWhole--;
        }
    }
    if (newValue != _value)
    {
        InvokeChangeEvent(newValue);
    }
}

void Spinner::UpHandler()
{
    money32 newValue = _value + SmallStep;
    if (newValue <= MaximumValue)
    {
        InvokeChangeEvent(newValue);
    }
}

void Spinner::DownHandler()
{
    money32 newValue = _value - SmallStep;
    if (newValue >= MinimumValue)
    {
        InvokeChangeEvent(newValue);
    }
}

void Spinner::InvokeChangeEvent(money32 value)
{
    if (ChangeEvent != nullptr)
    {
        ChangeEvent(value);
    }
}
