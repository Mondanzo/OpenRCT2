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

namespace OpenRCT2::Ui
{
    struct MouseEventArgs;
    class  Window;

    enum class VISIBILITY : uint8
    {
        VISIBLE,        // The widget is visible.
        HIDDEN,         // Space is reserved for the widget, but can not be seen or interacted with.
        COLLAPSED,      // No space is reserved for the widget in a widget container.
    };

    namespace WIDGET_FLAGS
    {
        constexpr uint16 AUTO_SIZE       = 1 << 0;
        constexpr uint16 STRETCH_H       = 1 << 1;
        constexpr uint16 STRETCH_V       = 1 << 2;
        constexpr uint16 ENABLED         = 1 << 3;
        constexpr uint16 FOCUSABLE       = 1 << 4;
        constexpr uint16 FOCUS           = 1 << 5;
        constexpr uint16 CURSOR          = 1 << 6;
        constexpr uint16 LAYOUT_DIRTY    = 1 << 7;
        constexpr uint16 VISUAL_DIRTY    = 1 << 8;
        constexpr uint16 INHERIT_STYLE   = 1 << 9;
    }

    struct Thickness
    {
        sint32 Top, Left, Right, Bottom;

        Thickness() : Thickness(0) { }
        Thickness(sint32 trbl) : Thickness(trbl, trbl, trbl, trbl) { }
        Thickness(sint32 tb, sint32 rl) : Thickness(tb, rl, tb, rl) { }
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
    private:
        Window *        _parentWindow = nullptr;
        VISIBILITY      _visibility = VISIBILITY::VISIBLE;
        uint8           _style = 0;
        rct_string_id   _defaultTooltip = (rct_string_id)-1;

    public:
        union
        {
            struct { sint32 X, Y, Width, Height; };
            struct { xy32 Location; size32 Size; };
            rect32 Bounds;
        };
        Thickness   Margin;
        uint16      Flags;

        Widget();
        virtual ~Widget() = default;

        virtual sint32 GetChildrenCount();
        virtual Widget * GetChild(sint32 index);

        virtual rct_string_id GetTooltip(sint32 x, sint32 y);

        Window * GetParentWindow();
        void SetParentWindow(Window * value);

        void SetEnabled(bool value);

        VISIBILITY GetVisibility() const;
        void SetVisibility(VISIBILITY value);

        uint8 GetStyle() const;
        void SetStyle(uint8 value);

        // Layout
        void InvalidateLayout();
        virtual void Measure() { }
        virtual void Arrange() { }

        void InvalidateVisual();
        virtual void Update() { };
        virtual void Draw(IDrawingContext * dc) { };

        // Interaction
        virtual bool HitTest(sint32 x, sint32 y);
        virtual void MouseDown(const MouseEventArgs * e) { };
        virtual void MouseMove(const MouseEventArgs * e) { };
        virtual void MouseUp(const MouseEventArgs * e) { };
        virtual void MouseWheel(const MouseEventArgs * e) { };

        // Helpers
        bool IsEnabled() { return (Flags & WIDGET_FLAGS::ENABLED) != 0; }
        bool IsDisabled() { return !IsEnabled(); }
        bool IsVisible() { return _visibility == VISIBILITY::VISIBLE; }
        size32 GetSizeWithMargin()
        {
            return { Width + Margin.Left + Margin.Right,
                     Height + Margin.Top + Margin.Bottom };
        }
        colour_t GetStyleColour();
    };
}
