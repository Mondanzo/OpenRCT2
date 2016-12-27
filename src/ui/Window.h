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

#include <string>
#include "../common.h"
#include "Primitives.h"
#include "WindowStyle.h"

#undef TRANSPARENT

interface IDrawingContext;

namespace OpenRCT2::Ui
{
    interface   ITabPanelAdapter;
    interface   IWindowManager;
    struct      MouseEventArgs;
    class       TabPanel;
    class       TitleBar;
    class       Widget;
    class       WindowShell;

    namespace WINDOW_FLAGS
    {
        constexpr uint32 AUTO_SIZE          = 1 << 0;
        constexpr uint32 FOCUS              = 1 << 1;
        constexpr uint32 CURSOR             = 1 << 2;
        constexpr uint32 MODAL              = 1 << 3;
        constexpr uint32 STICK_TO_BACK      = 1 << 4;
        constexpr uint32 STICK_TO_FRONT     = 1 << 5;
        constexpr uint32 TRANSPARENT        = 1 << 6;
        constexpr uint32 HAS_TITLE_BAR      = 1 << 7;
        constexpr uint32 HAS_CLOSE_BUTTON   = 1 << 8;
        constexpr uint32 HAS_TAB_PANEL      = 1 << 9;
        constexpr uint32 LAYOUT_DIRTY       = 1 << 10;
        constexpr uint32 RESIZING           = 1 << 11;
    }

    class Window
    {
    private:
        IWindowManager * _windowManager = nullptr;
        WindowShell * _windowShell = nullptr;

        Widget * _child = nullptr;
        Widget * _cursorWidget = nullptr;
        Widget * _focusWidget = nullptr;
        Widget * _holdWidget = nullptr;

        std::string         _title;
        ITabPanelAdapter *  _tabPanelAdapter = nullptr;

        xy32 _resizeCursorDelta;

        rect32 _bounds = { 0 };
        size32 _minimumSize = { 0 };
        size32 _maximumSize = { 0 };

        WindowStyle _style;
        uint32      _flags;

    public:
        Window();
        virtual ~Window();

        void SetWindowManager(IWindowManager * windowManager);
        void Initialise();

        Widget * GetWidgetAt(sint32 x, sint32 y);

        rect32 GetBounds() const;
        xy32 GetLocation() const;
        size32 GetSize() const;
        size32 GetMinimumSize() const;
        size32 GetMaximumSize() const;

        void SetBounds(rect32 bounds);
        void SetLocation(sint32 x, sint32 y);
        void SetLocation(xy32 location);
        void SetSize(sint32 width, sint32 height);
        void SetSize(size32 size);
        void SetMinimumSize(size32 size);
        void SetMaximumSize(size32 size);

        const WindowStyle * GetStyle() const;
        void SetStyle(const WindowStyle * style);

        uint32 GetFlags() const;
        bool HasFlag(uint32 flag) const;
        void SetFlags(uint32 flags);
        void SetFlag(uint32 flag, bool value);

        std::string GetTitle();
        void SetTitle(const std::string &title);

        ITabPanelAdapter * GetTabPanelAdapter();
        void SetTabPanelAdapter(ITabPanelAdapter * adapter);
        sint32 GetTabIndex();
        void SetTabIndex(sint32 index);
        void RefreshTabPanel();

        void Measure();
        void Arrange();

        void Invalidate();
        virtual void Update();
        virtual void Draw(IDrawingContext * g);

        virtual bool HitTest(sint32 x, sint32 y);
        virtual void MouseDown(const MouseEventArgs * e);
        virtual void MouseMove(const MouseEventArgs * e);
        virtual void MouseUp(const MouseEventArgs * e);
        virtual void MouseWheel(const MouseEventArgs * e);

        bool IsResizable();

    private:
        Widget * GetWidgetAt(Widget * node, sint32 x, sint32 y);
        void Measure(Widget * node);
        void Arrange(Widget * node);
        void Update(Widget * node, xy32 absolutePosition);
        void Draw(IDrawingContext * dc, Widget * node);
        void DrawSizeGrip(IDrawingContext * dc);

        void SetWidgetCursor(Widget * widget);
        void SetWidgetFocus(Widget * widget);

        rect32 GetResizeGripBounds();
        bool IsInResizeGripBounds(sint32 x, sint32 y);
    };
}
