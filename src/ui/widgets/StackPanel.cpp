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
#include "StackPanel.h"

using namespace OpenRCT2::Ui;

ORIENTATION StackPanel::GetOrientation() const
{
    return _orientation;
}

void StackPanel::SetOrientation(ORIENTATION value)
{
    _orientation = value;
}

void StackPanel::Measure()
{
    sint32 numChildren = GetChildrenCount();
    size32 minSize = { 0, 0 };
    size32 nonStretchSize = { 0, 0 };
    if (_orientation == ORIENTATION::HORIZONTAL)
    {
        for (sint32 i = 0; i < numChildren; i++)
        {
            Widget * widget = GetChild(i);
            if (widget->GetVisibility() != VISIBILITY::COLLAPSED)
            {
                size32 widgetSize = widget->GetSizeWithMargin();
                minSize.Width += widgetSize.Width;
                minSize.Height = Math::Max(minSize.Height, widgetSize.Height);
                if (!(widget->Flags & WIDGET_FLAGS::STRETCH_H))
                {
                    nonStretchSize.Width += widgetSize.Width;
                }
                if (!(widget->Flags & WIDGET_FLAGS::STRETCH_V))
                {
                    nonStretchSize.Height = Math::Max(nonStretchSize.Height, widgetSize.Height);
                }
            }
        }
    }
    else
    {
        for (sint32 i = 0; i < numChildren; i++)
        {
            Widget * widget = GetChild(i);
            if (widget->GetVisibility() != VISIBILITY::COLLAPSED)
            {
                size32 widgetSize = widget->GetSizeWithMargin();
                minSize.Width = Math::Max(minSize.Width, widgetSize.Width);
                minSize.Height += widgetSize.Height;
                if (!(widget->Flags & WIDGET_FLAGS::STRETCH_H))
                {
                    nonStretchSize.Width = Math::Max(nonStretchSize.Width, widgetSize.Width);
                }
                if (!(widget->Flags & WIDGET_FLAGS::STRETCH_V))
                {
                    nonStretchSize.Height += widgetSize.Height;
                }
            }
        }
    }

    Size = minSize;
    _measuredSize = nonStretchSize;
}

void StackPanel::Arrange()
{
    sint32 spareWidth = Width - _measuredSize.Width;
    sint32 spareHeight = Height - _measuredSize.Height;

    sint32 numChildren = GetChildrenCount();
    sint32 x = 0;
    sint32 y = 0;
    if (_orientation == ORIENTATION::HORIZONTAL)
    {
        for (sint32 i = 0; i < numChildren; i++)
        {
            Widget * widget = GetChild(i);
            if (widget->GetVisibility() != VISIBILITY::COLLAPSED)
            {
                widget->X = x + widget->Margin.Left;
                widget->Y = y + widget->Margin.Top;
                if (widget->Flags & WIDGET_FLAGS::STRETCH_H)
                {
                    sint32 marginWidth = widget->Margin.Left + widget->Margin.Right;
                    widget->Width = spareWidth - marginWidth;
                }
                if (widget->Flags & WIDGET_FLAGS::STRETCH_V)
                {
                    sint32 marginHeight = widget->Margin.Top + widget->Margin.Bottom;
                    widget->Height = Height - marginHeight;
                }
                x += widget->Margin.Left + widget->Width + widget->Margin.Right;
            }
        }
    }
    else
    {
        for (sint32 i = 0; i < numChildren; i++)
        {
            Widget * widget = GetChild(i);
            if (widget->GetVisibility() != VISIBILITY::COLLAPSED)
            {
                widget->X = x + widget->Margin.Left;
                widget->Y = y + widget->Margin.Top;
                if (widget->Flags & WIDGET_FLAGS::STRETCH_H)
                {
                    sint32 marginWidth = widget->Margin.Left + widget->Margin.Right;
                    widget->Width = Width - marginWidth;
                }
                if (widget->Flags & WIDGET_FLAGS::STRETCH_V)
                {
                    sint32 marginHeight = widget->Margin.Top + widget->Margin.Bottom;
                    widget->Height = spareHeight - marginHeight;
                }
                y += widget->Margin.Top + widget->Height + widget->Margin.Bottom;
            }
        }
    }
}

void StackPanel::Draw(IDrawingContext * dc)
{
#ifdef __DEBUG_STACKPANEL__
    uint8 colour = 21;
    dc->FillRect(colour, 0, 0, Width - 1, 0);
    dc->FillRect(colour, 0, Height - 1, Width - 1, Height - 1);
    dc->FillRect(colour, 0, 0, 0, Height - 1);
    dc->FillRect(colour, Width - 1, 0, Width - 1, Height - 1);
#endif
}
