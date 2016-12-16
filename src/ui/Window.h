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
    class       Button;
    interface   ITabPanelAdapter;
    struct      MouseEventArgs;
    class       TabPanel;
    class       TitleBar;
    class       Widget;

    namespace WINDOW_FLAGS
    {
        constexpr uint32 FOCUS              = 1 << 0;
        constexpr uint32 CURSOR             = 1 << 1;
        constexpr uint32 MODAL              = 1 << 2;
        constexpr uint32 STICK_TO_BACK      = 1 << 3;
        constexpr uint32 STICK_TO_FRONT     = 1 << 4;
        constexpr uint32 TRANSPARENT        = 1 << 5;
        constexpr uint32 HAS_TITLE_BAR      = 1 << 6;
        constexpr uint32 HAS_CLOSE_BUTTON   = 1 << 7;
        constexpr uint32 LAYOUT_DIRTY       = 1 << 8;
    }

    class Window
    {
    private:
        bool        _shimInitialised = false;
        TitleBar *  _titleBar = nullptr;
        Button *    _closeButton = nullptr;
        TabPanel *  _tabPanel = nullptr;

        Widget * _child = nullptr;
        Widget * _cursorWidget = nullptr;
        Widget * _focusWidget = nullptr;
        Widget * _holdWidget = nullptr;

        rct_string_id _title = (rct_string_id)-1;
        ITabPanelAdapter * _tabPanelAdapter = nullptr;

    public:
        union
        {
            struct { sint32 X, Y, Width, Height; };
            struct { xy32 Location; size32 Size; };
            rect32 Bounds;
        };
        size32 MinimumSize = { 0 };
        size32 MaximumSize = { 0 };
        uint32 Flags;
        uint32 BackgroundColour;

    public:
        Window();
        virtual ~Window();

        Widget * GetWidgetAt(sint32 x, sint32 y);

        rct_string_id GetTitle();
        void SetTitle(rct_string_id title);
        void SetTabPanelAdapter(ITabPanelAdapter * adapter);
        void SetTabIndex(sint32 index);

        void Measure();
        void Arrange();

        virtual void Update();
        virtual void Draw(IDrawingContext * g);

        virtual bool HitTest(sint32 x, sint32 y);
        virtual void MouseDown(const MouseEventArgs * e);
        virtual void MouseMove(const MouseEventArgs * e);
        virtual void MouseUp(const MouseEventArgs * e);
        virtual void MouseWheel(const MouseEventArgs * e);

    private:
        Widget * GetWidgetAt(Widget * node, sint32 x, sint32 y);
        void Measure(Widget * node);
        void Arrange(Widget * node);
        void Update(Widget * node);
        void Draw(IDrawingContext * g, Widget * node);

        void SetWidgetCursor(Widget * widget);
        void SetWidgetFocus(Widget * widget);

        void InitialiseShim();
        void ArrangeShim();
    };
}
