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

#include <functional>
#include "../Widget.h"

namespace OpenRCT2::Ui
{
    enum class BUTTON_TYPE : uint8
    {
        FLAT,
        OUTSET,
        IMAGE,
    };

    namespace BUTTON_FLAGS
    {
        constexpr uint8 STYLE_LIGHT     = 1 << 0;
        constexpr uint8 HIGHLIGHTED     = 1 << 1;
        constexpr uint8 PRESSED         = 1 << 2;
        constexpr uint8 DOWN            = 1 << 3;
        constexpr uint8 CLICK_REPEAT    = 1 << 4;
    }

    class Button : public Widget
    {
    private:
        uint32          _clickRepeatTimeout;

    public:
        uint8           ButtonFlags = 0;
        BUTTON_TYPE     Type = BUTTON_TYPE::FLAT;
        rct_string_id   Text = (rct_string_id)-1;
        uint32          Image = 0;
        uint32          ImageDown = 0;

        std::function<void()> ClickEvent = nullptr;

    public:
        void Measure() override;

        void Update() override;
        void Draw(IDrawingContext * dc) override;

        void MouseDown(const MouseEventArgs * e) override;
        void MouseUp(const MouseEventArgs * e) override;

    protected:
        bool IsHighlighted();
        bool IsPressed();
        bool IsDown();

    private:
        void DrawFlat(IDrawingContext * dc);
        void DrawOutset(IDrawingContext * dc);
        void DrawImage(IDrawingContext * dc);

        void InvokeClick();
    };
}
