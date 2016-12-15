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
#include "Panel.h"

namespace OpenRCT2 { namespace Ui
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
    class TabPanel : public Panel
    {
    private:
        ITabPanelAdapter * _adapter = nullptr;
        sint32 _selectedIndex = -1;
        bool _dirty = false;

    public:
        TabPanel();
        ~TabPanel();

        void SetAdapter(ITabPanelAdapter * adapter);
        void SetSelectedIndex(sint32 index);
        void Invalidate();

        void Update() override;
        void Draw(IDrawingContext * dc) override;

    private:
        void SetupWidgets();
    };
} }
