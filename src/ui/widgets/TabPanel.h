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

#include <vector>
#include "Button.h"
#include "Container.h"

namespace OpenRCT2::Ui
{
    struct TabImage;

    struct TabInfo
    {
        const TabImage *    Image;
        rct_string_id       ToolTip;
        uint8               Flags;
    };

    interface ITabPanelAdapter
    {
        virtual ~ITabPanelAdapter() = default;

        virtual sint32          GetTabCount() abstract;
        virtual const TabInfo * GetTabInfo(sint32 index) abstract;
        virtual Widget *        GetContent(sint32 index) abstract;
    };

    /**
     * Standard tab control which is present in most windows.
     */
    class TabPanel : public Widget
    {
    private:
        class Tab : public Button
        {
        public:
            TabPanel *  Parent = nullptr;
            sint32      Index = 0;
            TabInfo     Info;
            bool        Active = false;
            sint32      Offset = 0;
            sint32      FrameTimeout = 0;

        public:
            Tab();
            void Update() override;
            void Draw(IDrawingContext * dc) override;

        private:
            static void ClickHandler(Widget * sender, const void * e);
        };

    private:
        ITabPanelAdapter *  _adapter = nullptr;
        std::vector<Tab>    _tabs;
        Container           _container;

        sint32      _selectedIndex = -1;
        bool        _dirty = false;

    public:
        TabPanel();
        ~TabPanel();

        sint32 GetTabCount();
        void SetAdapter(ITabPanelAdapter * adapter);
        void SetSelectedIndex(sint32 index);
        void Invalidate();

        sint32 GetChildrenCount() override;
        Widget * GetChild(sint32 index) override;

        void Measure() override;
        void Arrange() override;

        void Update() override;
        void Draw(IDrawingContext * dc) override;

    private:
        void SetupWidgets();
    };
}
