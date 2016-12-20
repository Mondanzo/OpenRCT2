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

#include "../Widget.h"

namespace OpenRCT2::Ui
{
    class Button;

    namespace SPINNER_FLAGS
    {
        constexpr uint8 HIGH_PRECISION      = 1 << 0;
        constexpr uint8 SHOW_ZERO_AS_FREE   = 1 << 1;
        constexpr uint8 READ_ONLY           = 1 << 2;
    };

    /**
     * The title bar widget displaying the title of the window, seen in at
     * the top of most widgets. Moves the position of the window when dragged.
     */
    class Spinner : public Widget
    {
    private:
        Button * _upButton;
        Button * _downButton;
        money32  _value;

    public:
        uint8   Type = 0;
        uint8   SpinnerFlags = 0;
        money32 SmallStep = 1;
        money32 LargeStep = 5;
        money32 MinimumValue = 0;
        money32 MaximumValue = 100;

        std::function<void(money32)> ChangeEvent;

    public:
        Spinner();
        ~Spinner();

        money32 GetValue();
        void SetValue(money32 value);

        sint32 GetChildrenCount() override;
        Widget * GetChild(sint32 index) override;

        void Measure() override;
        void Arrange() override;

        void Update() override;
        void Draw(IDrawingContext * dc) override;

        void MouseWheel(const MouseEventArgs * e) override;

    private:
        void UpHandler();
        void DownHandler();
        void InvokeChangeEvent(money32 value);
    };
}
