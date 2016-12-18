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

#include "../core/Math.hpp"
#include "../drawing/IDrawingContext.h"
#include "../interface/Cursors.h"
#include "../localisation/string_ids.h"
#include "../sprites.h"
#include "DrawingContextExtensions.h"
#include "MouseEventArgs.h"
#include "widgets/Button.h"
#include "widgets/Panel.h"
#include "widgets/TabPanel.h"
#include "widgets/TitleBar.h"
#include "Window.h"
#include "WindowManager.h"
#include "WindowShell.h"

extern "C"
{
    extern colour_t gCurrentWindowColours[4];
}

using namespace OpenRCT2::Ui;

Window::Window()
{
    Bounds = { 0, 0, 0, 0 };
    Flags = WINDOW_FLAGS::AUTO_SIZE |
            WINDOW_FLAGS::HAS_TITLE_BAR |
            WINDOW_FLAGS::HAS_CLOSE_BUTTON |
            WINDOW_FLAGS::LAYOUT_DIRTY;

    _windowShell = new WindowShell(this);
}

Window::~Window()
{
    delete _windowShell;
}

void Window::SetWindowManager(IWindowManager * windowManager)
{
    _windowManager = windowManager;
}

void Window::Initialise()
{
    _windowShell->Initialise();
    _child = _windowShell;
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
        if (child != nullptr && child->IsVisible())
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

void Window::SetLocation(sint32 x, sint32 y)
{
    SetLocation({ x, y });
}

void Window::SetLocation(xy32 location)
{
    Invalidate();
    Location = location;
    Invalidate();
}

void Window::SetSize(sint32 width, sint32 height)
{
    SetSize({ width, height });
}

void Window::SetSize(size32 size)
{
    if (Size.Width != size.Width || Size.Height != size.Height)
    {
        Invalidate();
        Size = size;
        Flags |= WINDOW_FLAGS::LAYOUT_DIRTY;
        Invalidate();
    }
}

std::string Window::GetTitle()
{
    return _title;
}

void Window::SetTitle(const std::string &title)
{
    _title = title;

    auto titleBar = _windowShell->GetTitleBar();
    if (titleBar != nullptr)
    {
        titleBar->SetText(title);
    }
}

ITabPanelAdapter * Window::GetTabPanelAdapter()
{
    return _tabPanelAdapter;
}

void Window::SetTabPanelAdapter(ITabPanelAdapter * adapter)
{
    _tabPanelAdapter = adapter;

    auto tabPanel = _windowShell->GetTabPanel();
    if (tabPanel != nullptr)
    {
        tabPanel->SetAdapter(adapter);
    }
}

sint32 Window::GetTabIndex()
{
    sint32 index = -1;

    auto tabPanel = _windowShell->GetTabPanel();
    if (tabPanel != nullptr)
    {
        index = tabPanel->GetSelectedIndex();
    }
    return index;
}

void Window::SetTabIndex(sint32 index)
{
    auto tabPanel = _windowShell->GetTabPanel();
    if (tabPanel != nullptr)
    {
        tabPanel->SetSelectedIndex(index);
    }
}

void Window::Measure()
{
    if (_child != nullptr)
    {
        Measure(_child);

        size32 size;
        if ((Flags & WIDGET_FLAGS::AUTO_SIZE) || (Width == 0 && Height == 0))
        {
            size.Width = _child->Width;
            size.Height = _child->Height;
        }
        else
        {
            size.Width = Math::Clamp(MinimumSize.Width, Width, MaximumSize.Width);
            size.Height = Math::Clamp(MinimumSize.Height, Height, MaximumSize.Height);
        }
        SetSize(size);
    }
}

void Window::Measure(Widget * node)
{
    sint32 numChildren = node->GetChildrenCount();
    for (sint32 i = numChildren - 1; i >= 0; i--)
    {
        Widget * child = node->GetChild(i);
        if (child != nullptr)
        {
            Measure(child);
        }
    }

    node->Measure();
}

void Window::Arrange()
{
    if (_child != nullptr)
    {
        // Child fills window
        _child->X = 0;
        _child->Y = 0;
        _child->Width = Width;
        _child->Height = Height;

        // Recursively arrange the widget tree
        Arrange(_child);
    }
}

void Window::Arrange(Widget * node)
{
    node->Arrange();

    sint32 numChildren = node->GetChildrenCount();
    for (sint32 i = numChildren - 1; i >= 0; i--)
    {
        Widget * child = node->GetChild(i);
        if (child != nullptr)
        {
            Arrange(child);
        }
    }
}

void Window::Invalidate()
{
    if (_windowManager != nullptr)
    {
        _windowManager->Invalidate(Bounds);
    }
}

void Window::Update()
{
    if (_child != nullptr)
    {
        Update(_child, Location);
    }

    if (Flags & WINDOW_FLAGS::LAYOUT_DIRTY)
    {
        Flags &= ~WINDOW_FLAGS::LAYOUT_DIRTY;
        Invalidate();
        Measure();
        Arrange();
        Invalidate();
    }

    if (!(Flags & WINDOW_FLAGS::CURSOR))
    {
        // If the cursor is not over the window, it can't be over any
        // widget within the window.
        SetWidgetCursor(nullptr);
    }
}

void Window::Update(Widget * node, xy32 absolutePosition)
{
    node->ParentWindow = this;
    node->Update();

    sint32 numChildren = node->GetChildrenCount();
    for (sint32 i = numChildren - 1; i >= 0; i--)
    {
        Widget * child = node->GetChild(i);
        if (child != nullptr)
        {
            xy32 childAbsPosition = absolutePosition;
            childAbsPosition.X += child->X;
            childAbsPosition.Y += child->Y;

            if (child->Flags & WIDGET_FLAGS::INHERIT_STYLE)
            {
                if (child->Style != node->Style)
                {
                    child->Style = node->Style;
                    child->InvalidateVisual();
                }
            }

            Update(child, childAbsPosition);

            if (child->Flags & WIDGET_FLAGS::LAYOUT_DIRTY)
            {
                child->Flags &= ~WIDGET_FLAGS::LAYOUT_DIRTY;
                Flags |= WINDOW_FLAGS::LAYOUT_DIRTY;
            }
            if (child->Flags & WIDGET_FLAGS::VISUAL_DIRTY)
            {
                child->Flags &= ~WIDGET_FLAGS::VISUAL_DIRTY;

                rect32 widgetBounds = { childAbsPosition.X, childAbsPosition.Y, child->Width, child->Height };
                _windowManager->Invalidate(widgetBounds);
            }
        }
    }
}

void Window::Draw(IDrawingContext * dc)
{
    // HACK Legacy code for dealing with strings that use {WINDOW_COLOUR_X}
    for (sint32 i = 0; i < 4; i++)
    {
        gCurrentWindowColours[i] = NOT_TRANSLUCENT(Style.Colours[i]);
    }

    // Draw background
    uint32 bgColour = Style.Colours[0];
    uint8 press = 0;
    // uint8 press = (w->flags & WF_10 ? INSET_RECT_FLAG_FILL_MID_LIGHT : 0);
    press |= INSET_RECT_FLAG_FILL_MID_LIGHT;
    DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, bgColour, press);

    // Draw content
    if (_child != nullptr)
    {
        Draw(dc, _child);
    }

    // Draw size grip if the window can be resized
    if (IsResizable())
    {
        DrawSizeGrip(dc);
    }
}

void Window::Draw(IDrawingContext * dc, Widget * node)
{
    if (!node->IsVisible())
    {
        return;
    }

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


void Window::DrawSizeGrip(IDrawingContext * dc)
{
    uint32 colour = Style.Colours[0];
    if (Flags & WINDOW_FLAGS::HAS_TAB_PANEL)
    {
        colour = Style.Colours[1];
    }

    rect32 gripBounds = GetResizeGripBounds();
    uint32 sprite = SPR_RESIZE;
    sprite |= 0x20000000;
    sprite |= NOT_TRANSLUCENT(colour) << 19;
    dc->DrawSprite(sprite, gripBounds.X, gripBounds.Y, 0);
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
    Widget * widget = nullptr;
    if (e->Button == MOUSE_BUTTON::LEFT &&
        IsInResizeGripBounds(e->X, e->Y))
    {
        _resizeCursorDelta = { Width - e->X, Height - e->Y };
        Flags |= WINDOW_FLAGS::RESIZING;
    }
    else
    {
        widget = GetWidgetAt(e->X, e->Y);
    }

    SetWidgetFocus(widget);
    _holdWidget = widget;
    if (widget != nullptr)
    {
        MouseEventArgs e2 = e->CopyAndOffset(-widget->X, -widget->Y);
        widget->MouseDown(&e2);
    }
}

void Window::MouseMove(const MouseEventArgs * e)
{
    if (Flags & WINDOW_FLAGS::RESIZING)
    {
        xy32 cursorPos = { X + e->X, Y + e->Y };
        size32 newSize = {cursorPos.X + _resizeCursorDelta.X - X,
                          cursorPos.Y + _resizeCursorDelta.Y - Y };
        newSize.Width = Math::Clamp(MinimumSize.Width, newSize.Width, MaximumSize.Width);
        newSize.Height = Math::Clamp(MinimumSize.Height, newSize.Height, MaximumSize.Height);
        SetSize(newSize);
    }
    else
    {
        Widget * widget = GetWidgetAt(e->X, e->Y);
        Widget * cursorWidget = widget;

        // If we first pressed down on another widget, continue giving events to it
        if (_holdWidget != nullptr)
        {
            if (cursorWidget != _holdWidget)
            {
                cursorWidget = nullptr;
            }
            widget = _holdWidget;
        }

        SetWidgetCursor(cursorWidget);

        if (widget != nullptr)
        {
            MouseEventArgs e2 = e->CopyAndOffset(-widget->X, -widget->Y);
            widget->MouseMove(&e2);
        }

        if (IsInResizeGripBounds(e->X, e->Y))
        {
            Cursors::SetCurrentCursor(CURSOR_DIAGONAL_ARROWS);
        }
        else
        {
            // TODO get cursor from widget
            Cursors::SetCurrentCursor(CURSOR_ARROW);
        }
    }
}

void Window::MouseUp(const MouseEventArgs * e)
{
    if (Flags & WINDOW_FLAGS::RESIZING)
    {
        if (e->Button == MOUSE_BUTTON::LEFT)
        {
            Flags &= ~WINDOW_FLAGS::RESIZING;
        }
    }
    else
    {
        // If we first pressed down on another widget, continue giving events to it
        Widget * widget = _holdWidget;
        if (widget == nullptr)
        {
            widget = GetWidgetAt(e->X, e->Y);
        }
        if (widget != nullptr)
        {
            MouseEventArgs e2 = e->CopyAndOffset(-widget->X, -widget->Y);
            widget->MouseUp(&e2);
        }
        _holdWidget = nullptr;
    }
}

void Window::MouseWheel(const MouseEventArgs * e)
{
    Widget * widget = GetWidgetAt(e->X, e->Y);
    if (widget != nullptr)
    {
        MouseEventArgs e2 = e->CopyAndOffset(-widget->X, -widget->Y);
        widget->MouseWheel(&e2);
    }
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

bool Window::IsResizable()
{
    if (Flags & WINDOW_FLAGS::AUTO_SIZE)
    {
        return false;
    }
    if (MinimumSize.Width == MaximumSize.Width &&
        MinimumSize.Height == MaximumSize.Height)
    {
        return false;
    }
    return true;
}

rect32 Window::GetResizeGripBounds()
{
    rect32 bounds = { Width - 18, Height - 18, 18, 18 };
    return bounds;
}

bool Window::IsInResizeGripBounds(sint32 x, sint32 y)
{
    bool result = false;
    if (IsResizable())
    {
        rect32 gripBounds = GetResizeGripBounds();
        if (gripBounds.Contains(x, y))
        {
            result = true;
        }
    }
    return result;
}
