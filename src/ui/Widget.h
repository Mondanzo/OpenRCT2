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

#pragma once

#include "../common.h"
#include "Primitives.h"

interface IDrawingContext;

namespace OpenRCT2 { namespace Ui
{
    struct MouseEventArgs;

    enum class VISIBILITY : uint8
    {
        VISIBLE,        // The widget is visible.
        HIDDEN,         // Space is reserved for the widget, but can not be seen or interacted with.
        COLLAPSED,      // No space is reserved for the widget in a widget container.
    };

    namespace WIDGET_FLAGS
    {
        constexpr uint8 AUTO_SIZE       = 1 << 0;
        constexpr uint8 ENABLED         = 1 << 1;
        constexpr uint8 FOCUSABLE       = 1 << 2;
        constexpr uint8 FOCUS           = 1 << 3;
        constexpr uint8 CURSOR          = 1 << 4;
        constexpr uint8 LAYOUT_DIRTY    = 1 << 5;
        constexpr uint8 STRETCH_H       = 1 << 6;
        constexpr uint8 STRETCH_V       = 1 << 7;
    }

    struct Thickness
    {
        sint32 Top, Left, Right, Bottom;

        Thickness() { }
        Thickness(sint32 trbl)
        {
            Top = Left = Right = Bottom = trbl;
        }
        Thickness(sint32 tb, sint32 rl)
        {
            Top = Bottom = tb;
            Left = Right = rl;
        }
        Thickness(sint32 t, sint32 r, sint32 b, sint32 l)
        {
            Top = t;
            Right = r;
            Bottom = b;
            Left = l;
        }
    };

    class Widget
    {
    public:
        union
        {
            struct { sint32 X, Y, Width, Height; };
            struct { xy32 Location; size32 Size; };
            rect32 Bounds;
        };
        size32      DesiredSize;
        Thickness   Margin;
        uint8       Flags;
        VISIBILITY  Visibility;

        rct_string_id DefaultTooltip;

        Widget();
        virtual ~Widget() = default;

        virtual sint32 GetChildrenCount() { return 0; }
        virtual Widget * GetChild(sint32 index) { return nullptr; }

        virtual rct_string_id GetTooltip(sint32 x, sint32 y);

        // Layout
        void InvalidateLayout();
        virtual void Measure() { }
        virtual void Arrange() { }

        virtual void Update() { };
        virtual void Draw(IDrawingContext * dc) { };

        // Interaction
        virtual bool HitTest(sint32 x, sint32 y) { return true; }
        virtual void MouseDown(const MouseEventArgs * e) { };
        virtual void MouseMove(const MouseEventArgs * e) { };
        virtual void MouseUp(const MouseEventArgs * e) { };
        virtual void MouseWheel(const MouseEventArgs * e) { };
        virtual void MouseEnter(const MouseEventArgs * e) { };
        virtual void MouseLeave(const MouseEventArgs * e) { };

        // Helpers
        bool IsEnabled() { return (Flags & WIDGET_FLAGS::ENABLED) != 0; }
        bool IsDisabled() { return !IsEnabled(); }
        bool IsVisible() { return Visibility == VISIBILITY::VISIBLE; }
        size32 GetSizeWithMargin()
        {
            return { Width + Margin.Left + Margin.Right,
                     Height + Margin.Top + Margin.Bottom };
        }
    };
} }
