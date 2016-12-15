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

#include "../../drawing/IDrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../../sprites.h"
#include "../TabImages.h"
#include "../widgets/TabPanel.h"
#include "../Window.h"

extern "C"
{
    #include "../../interface/colour.h"
}

namespace OpenRCT2 { namespace Ui
{
    static TabInfo EntranceTabInfo  = { &TabImages::Park, STR_NONE, 0 };
    static TabInfo RatingTabInfo    = { &TabImages::RatingChart, STR_NONE, 0 };
    static TabInfo GuestsTabInfo    = { &TabImages::GuestChart, STR_NONE, 0 };
    static TabInfo AdmissionTabInfo = { &TabImages::Admission, STR_NONE, 0 };
    static TabInfo StatsTabInfo     = { &TabImages::Statistics, STR_NONE, 0 };
    static TabInfo ObjectiveTabInfo = { &TabImages::Objective, STR_NONE, 0 };
    static TabInfo AwardsTabInfo    = { &TabImages::Awards, STR_NONE, 0 };

    static const TabInfo * TabInfos[] =
    {
        &EntranceTabInfo,
        &RatingTabInfo,
        &GuestsTabInfo,
        &AdmissionTabInfo,
        &StatsTabInfo,
        &ObjectiveTabInfo,
        &AwardsTabInfo,
    };

    constexpr sint32 NUM_PAGES = 7;

    class ParkWindow : public Window,
                       public ITabPanelAdapter
    {
    public:
        ParkWindow()
        {
            X = 333;
            Y = 200;
            Width = 230;
            Height = 174 + 9;

            BackgroundColour = COLOUR_GREY;
            SetTitle(STR_PARK_CLOSED);
            SetTabPanelAdapter(this);
        }

        sint32 GetTabCount() override
        {
            return NUM_PAGES;
        }

        const TabInfo * GetTabInfo(sint32 index) override
        {
            return TabInfos[index];
        }

        Widget * GetContent(sint32 index) override
        {
            return nullptr;
        }
    };

    Window * OpenParkWindow()
    {
        return new ParkWindow();
    }
} }
