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
    #include "../../interface/colour.h"
    #include "../../world/park.h"
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

        rct_string_id   _statusArg;

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
            _status.Text = STR_BLACK_STRING;
            _status.TextArgs = &_statusArg;
            _statusArg = STR_PARK_OPEN;
            _grid0.AddChild(&_status);
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
        money32     _admissionsIncomeValue;

    public:
        AdmissionsPage()
        {
            _feeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_H;
            _feeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_V;
            _feeTextBlock.Margin.Left = 14;
            _feeTextBlock.Text = STR_ADMISSION_PRICE;

            _feeSpinner.SpinnerFlags |= SPINNER_FLAGS::HIGH_PRECISION |
                                        SPINNER_FLAGS::SHOW_ZERO_AS_FREE;
            _feeSpinner.Width = 76;
            _feeSpinner.Value = 1000;
            _feeSpinner.IncrementEvent = [this]() -> void {
                auto newFee = Math::Min(MONEY(100,00), gParkEntranceFee + MONEY(1,00));
                park_set_entrance_fee(newFee);
            };
            _feeSpinner.DecrementEvent = [this]() -> void {
                auto newFee = Math::Max(MONEY(0,00), gParkEntranceFee - MONEY(1,00));
                park_set_entrance_fee(newFee);
            };

            _grid1.Flags |= WIDGET_FLAGS::STRETCH_H;
            _grid1.SetOrientation(ORIENTATION::HORIZONTAL);
            _grid1.Margin = Thickness(6);
            _grid1.AddChild(&_feeTextBlock);
            _grid1.AddChild(&_feeSpinner);

            _admissionsIncomeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_H;
            _admissionsIncomeTextBlock.Margin = Thickness(6);
            _admissionsIncomeTextBlock.Text = STR_INCOME_FROM_ADMISSIONS;
            _admissionsIncomeTextBlock.TextArgs = &_admissionsIncomeValue;

            _grid0.SetOrientation(ORIENTATION::VERTICAL);
            _grid0.AddChild(&_grid1);
            _grid0.AddChild(&_admissionsIncomeTextBlock);

            SetChild(&_grid0);
        }

        void Update() override
        {
            if (_feeSpinner.Value != gParkEntranceFee)
            {
                _feeSpinner.Value = gParkEntranceFee;
            }
            if (_admissionsIncomeValue != gTotalIncomeFromAdmissions)
            {
                _admissionsIncomeValue = gTotalIncomeFromAdmissions;
                _admissionsIncomeTextBlock.InvalidateVisual();
            }
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
            X = 333;
            Y = 200;
            Width = 230;
            Height = 174 + 9;

            Flags |= WINDOW_FLAGS::HAS_TAB_PANEL;
            SetTitle(STR_PARK_CLOSED);
            SetTabPanelAdapter(this);

            Style.Colours[0] = COLOUR_GREY;
            Style.Colours[1] = COLOUR_DARK_YELLOW;
            Style.Colours[2] = COLOUR_DARK_YELLOW;
        }

        void Update() override
        {
            switch (GetTabIndex()) {
            case 0:
                Flags &= ~WINDOW_FLAGS::AUTO_SIZE;
                MinimumSize = { 230, 174 + 9 };
                MaximumSize = { 230 * 3, (274 + 9) * 3 };
                break;
            case 3:
                Flags &= ~WINDOW_FLAGS::AUTO_SIZE;
                Size = { 230, 124 };
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
            }

            return _currentPage;
        }
    };

    Window * OpenParkWindow()
    {
        return new ParkWindow();
    }
}
