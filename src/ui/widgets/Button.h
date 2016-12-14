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

#include "../Widget.h"

namespace OpenRCT2 { namespace Ui
{
    enum class BUTTON_STYLE : uint8
    {
        FLAT,
        OUTSET,
    };

    namespace BUTTON_FLAGS
    {
        constexpr uint8 HIGHLIGHTED = 1 << 0;
        constexpr uint8 PRESSED     = 1 << 1;
    }

    class Button : public Widget
    {
    private:
        BUTTON_STYLE    _style;
        uint8           _buttonFlags;

    public:
        uint32          Image = (uint32)-1;

    public:
        void Draw(IDrawingContext * dc) override;

        void MouseDown(const MouseEventArgs * e) override;
        void MouseUp(const MouseEventArgs * e) override;
        void MouseEnter(const MouseEventArgs * e) override;
        void MouseLeave(const MouseEventArgs * e) override;
    };
} }
