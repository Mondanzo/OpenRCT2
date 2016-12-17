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

using namespace OpenRCT2::Ui;

Widget::Widget()
{
    ParentWindow = nullptr;
    Bounds = { 0 };
    Flags = WIDGET_FLAGS::AUTO_SIZE |
            WIDGET_FLAGS::ENABLED |
            WIDGET_FLAGS::INHERIT_STYLE;
    Visibility = VISIBILITY::VISIBLE;
    Style = 0;
    DefaultTooltip = STR_NONE;
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
    return DefaultTooltip;
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
