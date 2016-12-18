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

#include "../../core/Math.hpp"
#include "../../drawing/IDrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../../sprites.h"
#include "../TabImages.h"
#include "../widgets/Button.h"
#include "../widgets/Container.h"
#include "../widgets/Spinner.h"
#include "../widgets/StackPanel.h"
#include "../widgets/TabPanel.h"
#include "../widgets/TextBlock.h"
#include "../widgets/Viewport.h"
#include "../Window.h"

extern "C"
{
    #include "../../config.h"
    #include "../../interface/colour.h"
    #include "../../localisation/localisation.h"
    #include "../../ride/ride.h"
    #include "../../util/util.h"
    #include "../../world/park.h"
}

std::string FormatLocaleString(rct_string_id stringId, const void * args = nullptr)
{
    utf8 buffer[4096];
    format_string(buffer, sizeof(buffer), stringId, (void *)args);
    return std::string(buffer);
}

namespace OpenRCT2::Ui
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

    class EntrancePage : public Container
    {
    private:
        StackPanel      _grid0;
        StackPanel      _grid1;
        ViewportWidget  _viewport;
        StackPanel      _toolbar;
        Button          _toolbarButtons[4];
        TextBlock       _status;

    public:
        EntrancePage()
        {
            Margin = Thickness(2, 0, 0, 2);

            _grid0.SetOrientation(ORIENTATION::VERTICAL);
            SetChild(&_grid0);

            _grid1.SetOrientation(ORIENTATION::HORIZONTAL);
            _grid1.Flags |= WIDGET_FLAGS::STRETCH_H;
            _grid1.Flags |= WIDGET_FLAGS::STRETCH_V;
            _grid0.AddChild(&_grid1);

            _viewport.Flags |= WIDGET_FLAGS::STRETCH_H;
            _viewport.Flags |= WIDGET_FLAGS::STRETCH_V;
            _grid1.AddChild(&_viewport);

            _toolbar.Margin = Thickness(3, 0, 0, 0);
            _toolbar.SetOrientation(ORIENTATION::VERTICAL);
            _grid1.AddChild(&_toolbar);

            for (int i = 0; i < 4; i++)
            {
                auto btn = &_toolbarButtons[i];
                btn->Type = BUTTON_TYPE::FLAT;
                _toolbar.AddChild(btn);
            }
            _toolbarButtons[0].Image = SPR_OPEN;
            _toolbarButtons[1].Image = SPR_BUY_LAND_RIGHTS;
            _toolbarButtons[2].Image = SPR_LOCATE;
            _toolbarButtons[3].Image = SPR_RENAME;

            _status.Flags |= WIDGET_FLAGS::STRETCH_H;
            _status.HorizontalAlignment = HORIZONTAL_ALIGNMENT::MIDDLE;
            _grid0.AddChild(&_status);
        }

        void Update() override
        {
            rct_string_id statusStringId = park_is_open() ? STR_PARK_OPEN : STR_PARK_CLOSED;
            _status.SetText(FormatLocaleString(STR_BLACK_STRING, &statusStringId));
        }
    };

    class AdmissionsPage : public Container
    {
    private:
        StackPanel  _grid0;
        StackPanel  _grid1;
        TextBlock   _feeTextBlock;
        Spinner     _feeSpinner;
        TextBlock   _admissionsIncomeTextBlock;
        money32     _admissionsIncomeValue = MONEY32_UNDEFINED;

    public:
        AdmissionsPage()
        {
            _feeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_H;
            _feeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_V;
            _feeTextBlock.Margin.Left = 14;
            _feeTextBlock.SetText(FormatLocaleString(STR_ADMISSION_PRICE));

            _feeSpinner.SpinnerFlags |= SPINNER_FLAGS::HIGH_PRECISION |
                                        SPINNER_FLAGS::SHOW_ZERO_AS_FREE;
            _feeSpinner.Width = 76;
            _feeSpinner.SmallStep = MONEY(1,00);
            _feeSpinner.MinimumValue = MONEY(0,00);
            _feeSpinner.MaximumValue = MONEY(100,00);
            _feeSpinner.ChangeEvent = [this](money32 value) -> void {
                park_set_entrance_fee(value);
            };

            _grid1.Flags |= WIDGET_FLAGS::STRETCH_H;
            _grid1.SetOrientation(ORIENTATION::HORIZONTAL);
            _grid1.Margin = Thickness(6);
            _grid1.AddChild(&_feeTextBlock);
            _grid1.AddChild(&_feeSpinner);

            _admissionsIncomeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_H;
            _admissionsIncomeTextBlock.Margin = Thickness(6);

            _grid0.SetOrientation(ORIENTATION::VERTICAL);
            _grid0.AddChild(&_grid1);
            _grid0.AddChild(&_admissionsIncomeTextBlock);

            SetChild(&_grid0);
        }

        void Update() override
        {
            bool feeLocked = ((gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices);
            if (feeLocked)
            {
                _feeSpinner.SpinnerFlags |= SPINNER_FLAGS::READ_ONLY;
            }
            else
            {
                _feeSpinner.SpinnerFlags &= ~SPINNER_FLAGS::READ_ONLY;
            }
            _feeSpinner.SetValue(gParkEntranceFee);

            if (_admissionsIncomeValue != gTotalIncomeFromAdmissions)
            {
                _admissionsIncomeValue = gTotalIncomeFromAdmissions;
                std::string text = FormatLocaleString(STR_INCOME_FROM_ADMISSIONS, &_admissionsIncomeValue);
                _admissionsIncomeTextBlock.SetText(text);
            }
        }
    };

    class StatsPage : public Container
    {
    private:
        StackPanel  _grid0;
        TextBlock   _textBlocks[5];

    public:
        StatsPage()
        {
            _grid0.SetOrientation(ORIENTATION::VERTICAL);
            _grid0.Margin = Thickness(3);
            SetChild(&_grid0);

            for (sint32 i = 0; i < 5; i++)
            {
                _textBlocks[i].Flags &= ~WIDGET_FLAGS::AUTO_SIZE;
                _textBlocks[i].Height = 10;
                _grid0.AddChild(&_textBlocks[i]);
            }
        }

        void Update() override
        {
            sint32 parkSize = gParkSize * 10;
            rct_string_id stringId = STR_PARK_SIZE_METRIC_LABEL;
            if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL)
            {
                stringId = STR_PARK_SIZE_IMPERIAL_LABEL;
                parkSize = squaredmetres_to_squaredfeet(parkSize);
            }
            _textBlocks[0].SetText(FormatLocaleString(stringId, &parkSize));

            sint16 numRides = ride_get_count();
            _textBlocks[1].SetText(FormatLocaleString(STR_NUMBER_OF_RIDES_LABEL, &numRides));

            sint16 numStaff = peep_get_staff_count();
            _textBlocks[2].SetText(FormatLocaleString(STR_STAFF_LABEL, &numStaff));

            _textBlocks[3].SetText(FormatLocaleString(STR_GUESTS_IN_PARK_LABEL, &gNumGuestsInPark));
            _textBlocks[4].SetText(FormatLocaleString(STR_TOTAL_ADMISSIONS, &gTotalAdmissions));
        }
    };

    class ParkWindow : public Window,
                       public ITabPanelAdapter
    {
    private:
        Widget * _currentPage = nullptr;

    public:
        ParkWindow()
        {
            SetLocation(333, 200);
            SetSize(230, 174 + 9);

            Flags |= WINDOW_FLAGS::HAS_TAB_PANEL;
            SetTabPanelAdapter(this);

            Style.Colours[0] = COLOUR_GREY;
            Style.Colours[1] = COLOUR_DARK_YELLOW;
            Style.Colours[2] = COLOUR_DARK_YELLOW;
        }

        void Update() override
        {
            SetTitle(FormatLocaleString(gParkName, &gParkNameArgs));

            switch (GetTabIndex()) {
            case 0:
                Flags &= ~WINDOW_FLAGS::AUTO_SIZE;
                MinimumSize = { 230, 174 + 9 };
                MaximumSize = { 230 * 3, (274 + 9) * 3 };
                break;
            case 3:
                Flags &= ~WINDOW_FLAGS::AUTO_SIZE;
                SetSize(230, 124);
                MinimumSize = Size;
                MaximumSize = Size;
                break;
            case 4:
                Flags &= ~WINDOW_FLAGS::AUTO_SIZE;
                SetSize(230, 109);
                MinimumSize = Size;
                MaximumSize = Size;
                break;
            default:
                Flags |= WINDOW_FLAGS::AUTO_SIZE;
                break;
            }

            Window::Update();
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
            delete _currentPage;
            _currentPage = nullptr;

            switch (index) {
            case 0:
                _currentPage = new EntrancePage();
                break;
            case 3:
                _currentPage = new AdmissionsPage();
                break;
            case 4:
                _currentPage = new StatsPage();
                break;
            }

            return _currentPage;
        }
    };

    Window * OpenParkWindow()
    {
        return new ParkWindow();
    }
}
