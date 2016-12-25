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
#include "Panel.h"

using namespace OpenRCT2::Ui;

void Panel::AddChild(Widget * child)
{
    Children.push_back(child);
}

void Panel::RemoveAllChildren()
{
    Children.clear();
}

sint32 Panel::GetChildrenCount()
{
    return (sint32)Children.size();
}

Widget * Panel::GetChild(sint32 index)
{
    Widget * result = nullptr;
    if (index >= 0 && index < (sint32)Children.size())
    {
        result = Children[index];
    }
    return result;
}

void Panel::Measure()
{
    if (Widget::Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        size32 size = { 0, 0 };
        for (Widget * widget : Children)
        {
            size.Width = Math::Max(size.Width, widget->X + widget->Width + widget->Margin.Right);
            size.Height = Math::Max(size.Height, widget->Y + widget->Height + widget->Margin.Bottom);
        }
        Size = size;
    }
}
