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

#include "../localisation/string_ids.h"
#include "Widget.h"
#include "Window.h"
#include "WindowStyle.h"

using namespace OpenRCT2::Ui;

Widget::Widget()
{
    Bounds = { 0 };
    Flags = WIDGET_FLAGS::AUTO_SIZE |
            WIDGET_FLAGS::ENABLED |
            WIDGET_FLAGS::INHERIT_STYLE;
}

sint32 Widget::GetChildrenCount()
{
    return 0;
}

Widget * Widget::GetChild(sint32 index)
{
    return nullptr;
}

rct_string_id Widget::GetTooltip(sint32 x, sint32 y)
{
    return _defaultTooltip;
}

Window * Widget::GetParentWindow()
{
    return _parentWindow;
}

void Widget::SetParentWindow(Window * value)
{
    _parentWindow = value;
}

void Widget::SetEnabled(bool value)
{
    if (IsEnabled() != value)
    {
        if (value)
        {
            Flags |= WIDGET_FLAGS::ENABLED;
        }
        else
        {
            Flags &= WIDGET_FLAGS::ENABLED;
        }
        InvalidateVisual();
    }
}

VISIBILITY Widget::GetVisibility() const
{
    return _visibility;
}

void Widget::SetVisibility(VISIBILITY value)
{
    if (value != _visibility)
    {
        if (value == VISIBILITY::COLLAPSED ||
            _visibility == VISIBILITY::COLLAPSED)
        {
            InvalidateLayout();
        }
        _visibility = value;
        InvalidateVisual();
    }
}

uint8 Widget::GetStyle() const
{
    return _style;
}

void Widget::SetStyle(uint8 value)
{
    _style = value;
}

void Widget::InvalidateLayout()
{
    Flags |= WIDGET_FLAGS::LAYOUT_DIRTY;
}

void Widget::InvalidateVisual()
{
    Flags |= WIDGET_FLAGS::VISUAL_DIRTY;
}

bool Widget::HitTest(sint32 x, sint32 y)
{
    return true;
}

colour_t Widget::GetStyleColour()
{
    colour_t colour = 0;
    if (_parentWindow != nullptr)
    {
        const WindowStyle * style = _parentWindow->GetStyle();
        colour = style->GetColour(_style);
    }
    return colour;
}
