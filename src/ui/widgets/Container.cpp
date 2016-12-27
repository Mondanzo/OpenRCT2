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

#include "Container.h"

using namespace OpenRCT2::Ui;

Widget * Container::GetChild()
{
    return _child;
}

void Container::SetChild(Widget * widget)
{
    _child = widget;
}

sint32 Container::GetChildrenCount()
{
    return (_child == nullptr) ? 0 : 1;
}

Widget * Container::GetChild(sint32 index)
{
    Widget * result = nullptr;
    if (index == 0)
    {
        return _child;
    }
    return result;
}

void Container::Measure()
{
    if (Widget::Flags & WIDGET_FLAGS::AUTO_SIZE)
    {
        size32 size = { 0, 0 };
        Widget * child = _child;
        if (child != nullptr)
        {
            size = child->GetSizeWithMargin();
        }
        SetSize(size);
    }
}

void Container::Arrange()
{
    Widget * child = _child;
    if (child != nullptr)
    {
        Thickness margin = child->GetMargin();
        child->SetX(margin.Left);
        child->SetY(margin.Top);
        child->SetWidth(GetWidth() - margin.Left - margin.Right);
        child->SetHeight(GetHeight() - margin.Top - margin.Bottom);
    }
}
