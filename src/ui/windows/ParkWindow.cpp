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
#include "../../drawing/DrawingContext.h"
#include "../../localisation/string_ids.h"
#include "../../sprites.h"
#include "../TabImages.h"
#include "../widgets/Button.h"
#include "../widgets/Chart.h"
#include "../widgets/Container.h"
#include "../widgets/Spinner.h"
#include "../widgets/StackPanel.h"
#include "../widgets/TabPanel.h"
#include "../widgets/TextBlock.h"
#include "../widgets/Viewport.h"
#include "../Window.h"
#include "../WindowManager.h"

extern "C"
{
    #include "../../config.h"
    #include "../../interface/colour.h"
    #include "../../localisation/localisation.h"
    #include "../../management/award.h"
    #include "../../ride/ride.h"
    #include "../../scenario/scenario.h"
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
    enum PAGE
    {
        PAGE_ENTRANCE,
        PAGE_RATING,
        PAGE_GUESTS,
        PAGE_ADMISSION,
        PAGE_STATS,
        PAGE_OBJECTIVE,
        PAGE_AWARDS,
    };
    constexpr sint32 NUM_PAGES = 7;

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
            SetMargin(Thickness(2, 0, 0, 2));

            _grid0.SetOrientation(ORIENTATION::VERTICAL);
            SetChild(&_grid0);

            _grid1.SetOrientation(ORIENTATION::HORIZONTAL);
            _grid1.Flags |= WIDGET_FLAGS::STRETCH_H;
            _grid1.Flags |= WIDGET_FLAGS::STRETCH_V;
            _grid0.AddChild(&_grid1);

            _viewport.Flags |= WIDGET_FLAGS::STRETCH_H;
            _viewport.Flags |= WIDGET_FLAGS::STRETCH_V;
            _grid1.AddChild(&_viewport);

            _toolbar.SetMargin(Thickness(3, 0, 0, 0));
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

            xyz32 viewLocation = { 0 };
            for (sint32 i = 0; i < 4; i++) {
                if (gParkEntranceX[i] != SPRITE_LOCATION_NULL) {
                    viewLocation.X = gParkEntranceX[i] + 16;
                    viewLocation.Y = gParkEntranceY[i] + 16;
                    viewLocation.Z = gParkEntranceZ[i] + 32;
                    break;
                }
            }
            _viewport.SetLocation(viewLocation);
        }
    };

    class RatingPage : public Container
    {
    private:
        StackPanel  _grid;
        TextBlock   _ratingTextBlock;
        Chart       _chart;

    public:
        RatingPage()
        {
            SetMargin(Thickness(3));

            _chart.Flags |= WIDGET_FLAGS::STRETCH_H |
                            WIDGET_FLAGS::STRETCH_V;

            _grid.SetOrientation(ORIENTATION::VERTICAL);
            _grid.AddChild(&_ratingTextBlock);
            _grid.AddChild(&_chart);
            SetChild(&_grid);
        }

        void Update() override
        {
            _ratingTextBlock.SetText(FormatLocaleString(STR_PARK_RATING_LABEL, &gParkRating));
            _chart.SetValues(gParkRatingHistory, 32);
        }
    };

    class GuestsPage : public Container
    {
    private:
        StackPanel  _grid;
        TextBlock   _guestsTextBlock;
        Chart       _chart;

    public:
        GuestsPage()
        {
            SetMargin(Thickness(3));

            _chart.Flags |= WIDGET_FLAGS::STRETCH_H |
                            WIDGET_FLAGS::STRETCH_V;

            _grid.SetOrientation(ORIENTATION::VERTICAL);
            _grid.AddChild(&_guestsTextBlock);
            _grid.AddChild(&_chart);
            SetChild(&_grid);
        }

        void Update() override
        {
            _guestsTextBlock.SetText(FormatLocaleString(STR_GUESTS_IN_PARK_LABEL, &gNumGuestsInPark));
            _chart.SetValues(gGuestsInParkHistory, 32);
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
            _feeTextBlock.SetMargin(Thickness(0, 0, 0, 14));
            _feeTextBlock.SetText(FormatLocaleString(STR_ADMISSION_PRICE));

            _feeSpinner.SpinnerFlags |= SPINNER_FLAGS::HIGH_PRECISION |
                                        SPINNER_FLAGS::SHOW_ZERO_AS_FREE;
            _feeSpinner.SetWidth(76);
            _feeSpinner.SmallStep = MONEY(1,00);
            _feeSpinner.MinimumValue = MONEY(0,00);
            _feeSpinner.MaximumValue = MONEY(100,00);
            _feeSpinner.ChangeEvent = [this](money32 value) -> void {
                park_set_entrance_fee(value);
            };

            _grid1.Flags |= WIDGET_FLAGS::STRETCH_H;
            _grid1.SetOrientation(ORIENTATION::HORIZONTAL);
            _grid1.SetMargin(Thickness(6));
            _grid1.AddChild(&_feeTextBlock);
            _grid1.AddChild(&_feeSpinner);

            _admissionsIncomeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_H;
            _admissionsIncomeTextBlock.SetMargin(Thickness(6));

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
            _grid0.SetMargin(Thickness(3));
            SetChild(&_grid0);

            for (sint32 i = 0; i < 5; i++)
            {
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

    class ObjectivePage : public Container
    {
    private:
        StackPanel  _grid;
        TextBlock   _detailsTextBlock;
        TextBlock   _objectiveLabelTextBlock;
        TextBlock   _objectiveTextBlock;
        TextBlock   _outcomeTextBlock;
        Button      _enterNameButton;

    public:
        ObjectivePage()
        {
            SetMargin(Thickness(4));

            _detailsTextBlock.SetWrapping(true);
            _detailsTextBlock.VerticalAlignment = VERTICAL_ALIGNMENT::TOP;
            _detailsTextBlock.SetMargin(Thickness(0, 0, 5, 0));

            _objectiveLabelTextBlock.SetText(FormatLocaleString(STR_OBJECTIVE_LABEL));
            _objectiveLabelTextBlock.VerticalAlignment = VERTICAL_ALIGNMENT::TOP;
            _detailsTextBlock.SetMargin(Thickness(0, 0, 10, 0));

            _objectiveTextBlock.SetWrapping(true);
            _objectiveTextBlock.VerticalAlignment = VERTICAL_ALIGNMENT::TOP;

            _outcomeTextBlock.Flags |= WIDGET_FLAGS::STRETCH_V;
            _outcomeTextBlock.SetWrapping(true);
            _outcomeTextBlock.VerticalAlignment = VERTICAL_ALIGNMENT::TOP;

            _enterNameButton.Flags |= WIDGET_FLAGS::STRETCH_H;
            _enterNameButton.Type = BUTTON_TYPE::OUTSET;
            _enterNameButton.Text = STR_ENTER_NAME_INTO_SCENARIO_CHART;
            _enterNameButton.SetMargin(Thickness(0, 2));

            _grid.SetOrientation(ORIENTATION::VERTICAL);
            _grid.AddChild(&_detailsTextBlock);
            _grid.AddChild(&_objectiveLabelTextBlock);
            _grid.AddChild(&_objectiveTextBlock);
            _grid.AddChild(&_outcomeTextBlock);
            _grid.AddChild(&_enterNameButton);
            SetChild(&_grid);
        }

        void Update() override
        {
            // Details
            set_format_arg(0, rct_string_id, STR_STRING);
            set_format_arg(2, const char *, gScenarioDetails);
            _detailsTextBlock.SetText(FormatLocaleString(STR_BLACK_STRING, gCommonFormatArgs));

            // Objective
            rct_string_id stringId = ObjectiveNames[gScenarioObjectiveType];
            set_format_arg(0, uint16, gScenarioObjectiveNumGuests);
            set_format_arg(2, sint16, date_get_total_months(MONTH_OCTOBER, gScenarioObjectiveYear));
            set_format_arg(4, money32, gScenarioObjectiveCurrency);
            _objectiveTextBlock.SetText(FormatLocaleString(stringId, gCommonFormatArgs));

            // Objective outcome
            if (gScenarioCompletedCompanyValue != MONEY32_UNDEFINED)
            {
                _outcomeTextBlock.SetVisibility(VISIBILITY::VISIBLE);
                if (gScenarioCompletedCompanyValue == 0x80000001)
                {
                    // Objective failed
                    _outcomeTextBlock.SetText(FormatLocaleString(STR_OBJECTIVE_FAILED));
                }
                else
                {
                    // Objective completed
                    set_format_arg(0, money32, gScenarioCompletedCompanyValue);
                    _outcomeTextBlock.SetText(FormatLocaleString(STR_OBJECTIVE_ACHIEVED, gCommonFormatArgs));
                }
                if (gParkFlags & PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT)
                {
                    _enterNameButton.SetVisibility(VISIBILITY::VISIBLE);
                }
                else
                {
                    _enterNameButton.SetVisibility(VISIBILITY::COLLAPSED);
                }
            }
            else
            {
                _outcomeTextBlock.SetVisibility(VISIBILITY::COLLAPSED);
                _enterNameButton.SetVisibility(VISIBILITY::COLLAPSED);
            }
        }
    };

    class AwardsPage : public Widget
    {
        struct ParkAward
        {
            rct_string_id   Text;
            uint32          Sprite;
        };
        static constexpr ParkAward ParkAwards[] = {
            { STR_AWARD_MOST_UNTIDY,                SPR_AWARD_MOST_UNTIDY },
            { STR_AWARD_MOST_TIDY,                  SPR_AWARD_MOST_TIDY },
            { STR_AWARD_BEST_ROLLERCOASTERS,        SPR_AWARD_BEST_ROLLERCOASTERS },
            { STR_AWARD_BEST_VALUE,                 SPR_AWARD_BEST_VALUE },
            { STR_AWARD_MOST_BEAUTIFUL,             SPR_AWARD_MOST_BEAUTIFUL },
            { STR_AWARD_WORST_VALUE,                SPR_AWARD_WORST_VALUE },
            { STR_AWARD_SAFEST,                     SPR_AWARD_SAFEST },
            { STR_AWARD_BEST_STAFF,                 SPR_AWARD_BEST_STAFF },
            { STR_AWARD_BEST_FOOD,                  SPR_AWARD_BEST_FOOD },
            { STR_AWARD_WORST_FOOD,                 SPR_AWARD_WORST_FOOD },
            { STR_AWARD_BEST_RESTROOMS,             SPR_AWARD_BEST_RESTROOMS },
            { STR_AWARD_MOST_DISAPPOINTING,         SPR_AWARD_MOST_DISAPPOINTING },
            { STR_AWARD_BEST_WATER_RIDES,           SPR_AWARD_BEST_WATER_RIDES },
            { STR_AWARD_BEST_CUSTOM_DESIGNED_RIDES, SPR_AWARD_BEST_CUSTOM_DESIGNED_RIDES },
            { STR_AWARD_MOST_DAZZLING_RIDE_COLOURS, SPR_AWARD_MOST_DAZZLING_RIDE_COLOURS },
            { STR_AWARD_MOST_CONFUSING_LAYOUT,      SPR_AWARD_MOST_CONFUSING_LAYOUT },
            { STR_AWARD_BEST_GENTLE_RIDES,          SPR_AWARD_BEST_GENTLE_RIDES },
        };

        void Draw(IDrawingContext * dc)
        {
            uintptr_t dpip = ((uintptr_t *)dc)[2];
            rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;

            sint32 x = 4;
            sint32 y = 4;
            sint32 count = 0;
            for (sint32 i = 0; i < MAX_AWARDS; i++) {
                const rct_award * award = &gCurrentAwards[i];
                if (award->time != 0)
                {
                    const ParkAward * padesc = &ParkAwards[award->type];
                    dc->DrawSprite(padesc->Sprite, x, y, 0);
                    gfx_draw_string_left_wrapped(dpi, nullptr, x + 34, y + 6, 180, padesc->Text, COLOUR_BLACK);

                    y += 32;
                    count++;
                }
            }

            if (count == 0)
            {
                gfx_draw_string_left(dpi, STR_NO_RECENT_AWARDS, nullptr, COLOUR_BLACK, x + 6, y + 6);
            }
        }
    };

    class ParkWindow : public Window,
                       public ITabPanelAdapter
    {
    private:
        static constexpr size32 PageWindowSizes[] = {
            { 230, 174 + 9 },   // PAGE_ENTRANCE
            { 230, 182 },       // PAGE_RATING
            { 230, 182 },       // PAGE_GUESTS
            { 230, 124 },       // PAGE_ADMISSION
            { 230, 109 },       // PAGE_STATS
            { 230, 224 },       // PAGE_OBJECTIVE
            { 230, 182 },       // PAGE_AWARDS
        };

        Widget *    _currentPage = nullptr;
        bool        _moneyDisabled = false;

    public:
        ParkWindow()
        {
            SetLocation(333, 200);
            SetSize(230, 174 + 9);

            SetFlag(WINDOW_FLAGS::HAS_TAB_PANEL, true);
            SetTabPanelAdapter(this);

            WindowStyle style;
            style.Colours[0] = COLOUR_GREY;
            style.Colours[1] = COLOUR_DARK_YELLOW;
            style.Colours[2] = COLOUR_DARK_YELLOW;
            SetStyle(&style);
        }

        void Update() override
        {
            bool moneyDisabled = ((gParkFlags & PARK_FLAGS_NO_MONEY) != 0);
            if (moneyDisabled != _moneyDisabled)
            {
                _moneyDisabled = moneyDisabled;
                RefreshTabPanel();
            }

            SetTitle(FormatLocaleString(gParkName, &gParkNameArgs));

            sint32 page = GetTabIndex();
            switch (page) {
            case PAGE_ENTRANCE:
                SetFlag(WINDOW_FLAGS::AUTO_SIZE, false);
                SetMinimumSize({ 230, 174 + 9 });
                SetMaximumSize({ 230 * 3, (274 + 9) * 3 });
                break;
            case PAGE_RATING:
            case PAGE_GUESTS:
            case PAGE_ADMISSION:
            case PAGE_OBJECTIVE:
            case PAGE_AWARDS:
                SetFlag(WINDOW_FLAGS::AUTO_SIZE, false);
                SetSize(PageWindowSizes[page]);
                SetMinimumSize(GetSize());
                SetMaximumSize(GetSize());
                break;
            case PAGE_STATS:
            default:
                SetFlag(WINDOW_FLAGS::AUTO_SIZE, true);
                SetMinimumSize(PageWindowSizes[page]);
                SetMaximumSize(SIZE32_MAX);
                break;
            }

            Window::Update();
        }

        sint32 GetTabCount() override
        {
            return NUM_PAGES;
        }

        TabInfo GetTabInfo(sint32 index) override
        {
            TabInfo tabInfo = *TabInfos[index];
            if (index == PAGE_ADMISSION && (gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                tabInfo.Flags |= TABINFO_FLAGS::HIDDEN;
            }
            return tabInfo;
        }

        Widget * GetContent(sint32 index) override
        {
            delete _currentPage;
            _currentPage = nullptr;

            switch (index) {
            case PAGE_ENTRANCE:
                _currentPage = new EntrancePage();
                break;
            case PAGE_RATING:
                _currentPage = new RatingPage();
                break;
            case PAGE_GUESTS:
                _currentPage = new GuestsPage();
                break;
            case PAGE_ADMISSION:
                _currentPage = new AdmissionsPage();
                break;
            case PAGE_STATS:
                _currentPage = new StatsPage();
                break;
            case PAGE_OBJECTIVE:
                _currentPage = new ObjectivePage();
                break;
            case PAGE_AWARDS:
                _currentPage = new AwardsPage();
                break;
            }

            return _currentPage;
        }
    };

    Window * OpenParkWindow()
    {
        Window * window = new ParkWindow();
        IWindowManager * wm = GetWindowManager();
        wm->ShowWindow(window);
        return window;
    }
}
