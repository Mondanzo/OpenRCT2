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
#include "../localisation/string_ids.h"
#include "../sprites.h"
#include "DrawingContextExtensions.h"
#include "MouseEventArgs.h"
#include "widgets/Button.h"
#include "widgets/Panel.h"
#include "widgets/TabPanel.h"
#include "widgets/TitleBar.h"
#include "Window.h"

using namespace OpenRCT2::Ui;

Window::Window()
{
    Bounds = { 0, 0, 0, 0 };
    BackgroundColour = COLOUR_GREY;
    Flags = WINDOW_FLAGS::HAS_TITLE_BAR |
            WINDOW_FLAGS::HAS_CLOSE_BUTTON;
}

Window::~Window()
{
    delete _tabPanel;
    delete _closeButton;
    delete _titleBar;
    delete _child;
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

rct_string_id Window::GetTitle()
{
    return _title;
}

void Window::SetTitle(rct_string_id title)
{
    _title = title;
    if (_titleBar != nullptr)
    {
        _titleBar->Text = title;
    }
}

void Window::SetTabPanelAdapter(ITabPanelAdapter * adapter)
{
    _tabPanelAdapter = adapter;
    if (_tabPanel != nullptr)
    {
        _tabPanel->SetAdapter(adapter);
    }
}

void Window::SetTabIndex(sint32 index)
{
    if (_tabPanel != nullptr)
    {
        _tabPanel->SetSelectedIndex(index);
    }
}

void Window::Update()
{
    if (!_shimInitialised)
    {
        _shimInitialised = true;
        InitialiseShim();
    }

    // TODO Move this block to layout event
    if (_titleBar != nullptr)
    {
        _titleBar->Width = Width - 2;
    }
    if (_closeButton != nullptr)
    {
        _closeButton->X = Width - 13;
    }
    if (_tabPanel != nullptr)
    {
        _tabPanel->Width = Width - _tabPanel->X;
        _tabPanel->Height = Height - _tabPanel->Y;
    }
    if (_child != nullptr)
    {
        _child->Width = Bounds.Width;
        _child->Height = Bounds.Height;
    }

    if (_child != nullptr)
    {
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
    uint8 press = 0;
    // uint8 press = (w->flags & WF_10 ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);
    press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, BackgroundColour, press);

    // Check if the window can be resized
    if (MinimumSize.Width != MaximumSize.Width && MinimumSize.Height == MaximumSize.Height)
    {
        // Draw the resize sprite at the bottom right corner
        sint32 l = Width - 18;
        sint32 t = Height - 18;
        dc->DrawSprite(SPR_RESIZE | 0x20000000 | ((BackgroundColour & 0x7F) << 19), l, t, 0);
    }

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
        for (sint32 i = 0; i < numChildren; i++)
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

void Window::InitialiseShim()
{
    // Create root container
    auto panel = new Panel();
    panel->X = 0;
    panel->Y = 0;
    panel->Width = Bounds.Width;
    panel->Height = Bounds.Height;
    _child = panel;

    // Title bar
    if (Flags & WINDOW_FLAGS::HAS_TITLE_BAR)
    {
        _titleBar = new TitleBar();
        _titleBar->X = 1;
        _titleBar->Y = 1;
        _titleBar->Height = 14;
        _titleBar->Text = _title;
        panel->AddChild(_titleBar);
    }

    // Close button
    if (Flags & WINDOW_FLAGS::HAS_TITLE_BAR)
    {
        _closeButton = new Button();
        _closeButton->X = Width - 13;
        _closeButton->Y = 2;
        _closeButton->Width = 11;
        _closeButton->Height = 12;
        _closeButton->Text = STR_CLOSE_X;
        _closeButton->Style = BUTTON_STYLE::OUTSET;
        panel->AddChild(_closeButton);
    }

    _tabPanel = new TabPanel();
    _tabPanel->X;
    _tabPanel->Y = 17;
    _tabPanel->Width = Width;
    _tabPanel->Height = Height - _tabPanel->Y;
    _tabPanel->SetAdapter(_tabPanelAdapter);
    panel->AddChild(_tabPanel);
}
