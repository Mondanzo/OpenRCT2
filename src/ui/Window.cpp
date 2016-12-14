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

#include "../drawing/IDrawingContext.h"
#include "MouseEventArgs.h"
#include "widgets/Button.h"
#include "widgets/Panel.h"
#include "Window.h"

using namespace OpenRCT2::Ui;

Window::Window()
{
    auto panel = new Panel();
    panel->X = 0;
    panel->Y = 0;
    panel->Width = Bounds.Width;
    panel->Height = Bounds.Height;

    auto btn = new Button();
    btn->X = 12;
    btn->Y = 12;
    btn->Width = 15;
    btn->Height = 15;
    panel->AddChild(btn);

    btn = new Button();
    btn->X = 33;
    btn->Y = 17;
    btn->Width = 23;
    btn->Height = 23;
    panel->AddChild(btn);

    _child = panel;
}

Widget * Window::GetWidgetAt(sint32 x, sint32 y)
{
    Widget * result = nullptr;
    if (_child != nullptr)
    {
        result = GetWidgetAt(_child, x, y);
    }
    return result;
}

Widget * Window::GetWidgetAt(Widget * node, sint32 x, sint32 y)
{
    // Check children
    sint32 numChildren = node->GetChildrenCount();
    for (sint32 i = numChildren - 1; i >= 0; i--)
    {
        Widget * child = node->GetChild(i);
        if (child != nullptr)
        {
            rect32 bounds = child->Bounds;
            if (bounds.Contains(x, y))
            {
                // Recurse
                sint32 relX = x - bounds.X;
                sint32 relY = y - bounds.Y;
                Widget * widget = GetWidgetAt(child, relX, relY);
                if (widget != nullptr)
                {
                    return widget;
                }
            }
        }
    }

    // Check self
    if (node->HitTest(x, y))
    {
        return node;
    }

    // No widget here
    return nullptr;
}

void Window::Update()
{
    if (_child != nullptr)
    {
        _child->Width = Bounds.Width;
        _child->Height = Bounds.Height;
        Update(_child);
    }
}

void Window::Update(Widget * node)
{
    node->Update();

    sint32 numChildren = node->GetChildrenCount();
    for (sint32 i = numChildren - 1; i >= 0; i--)
    {
        Widget * child = node->GetChild(i);
        if (child != nullptr)
        {
            Update(child);
        }
    }
}

void Window::Draw(IDrawingContext * dc)
{
    uint32 colour = 72;
    if (Flags & WINDOW_FLAGS::FOCUS)
    {
        colour = 152;
    }
    dc->FillRect(colour, 0, 0, Width, Height);

    if (_child != nullptr)
    {
        Draw(dc, _child);
    }
}

void Window::Draw(IDrawingContext * dc, Widget * node)
{
    IDrawingContext * dc2 = dc->Nest(node->X, node->Y, node->Width, node->Height);
    if (dc2 != nullptr)
    {
        node->Draw(dc2);

        sint32 numChildren = node->GetChildrenCount();
        for (sint32 i = numChildren - 1; i >= 0; i--)
        {
            Widget * child = node->GetChild(i);
            if (child != nullptr)
            {
                Draw(dc2, child);
            }
        }
    }
}

bool Window::HitTest(sint32 x, sint32 y)
{
    bool result = true;
    bool isTransparent = (Flags & WINDOW_FLAGS::TRANSPARENT) != 0;
    if (isTransparent)
    {
        // Need to check if a widget is at this position
        Widget * widget = GetWidgetAt(x, y);
        result = widget != nullptr;
    }
    return result;
}

void Window::MouseDown(const MouseEventArgs * e)
{
    Widget * widget = GetWidgetAt(e->X, e->Y);
    if (widget != nullptr)
    {
        SetWidgetFocus(widget);

        MouseEventArgs e2 = e->CopyAndOffset(-widget->X, -widget->Y);
        widget->MouseDown(&e2);
    }
}

void Window::MouseMove(const MouseEventArgs * e)
{
}

void Window::MouseUp(const MouseEventArgs * e)
{
}

void Window::MouseWheel(const MouseEventArgs * e)
{
}

void Window::SetWidgetCursor(Widget * widget)
{
    // Unset, set cursor flag
    if (_cursorWidget != nullptr)
    {
        _cursorWidget->Flags &= ~WIDGET_FLAGS::CURSOR;
    }
    _cursorWidget = widget;
    if (widget != nullptr)
    {
        widget->Flags |= WIDGET_FLAGS::CURSOR;
    }
}

void Window::SetWidgetFocus(Widget * widget)
{
    // Unset, set focus flag
    if (_focusWidget != nullptr)
    {
        _focusWidget->Flags &= ~WIDGET_FLAGS::FOCUS;
    }
    _focusWidget = widget;
    if (widget != nullptr)
    {
        widget->Flags |= WIDGET_FLAGS::FOCUS;
    }
}
