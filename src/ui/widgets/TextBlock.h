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
    enum class HORIZONTAL_ALIGNMENT : uint8
    {
        LEFT,
        MIDDLE,
        RIGHT,
    };

    enum class VERTICAL_ALIGNMENT : uint8
    {
        TOP,
        MIDDLE,
        BOTTOM,
    };

    class TextBlock : public Widget
    {
    public:
        HORIZONTAL_ALIGNMENT    HorizontalAlignment;
        HORIZONTAL_ALIGNMENT    VerticalAlignment;
        rct_string_id           Text = (rct_string_id)-1;
        void *                  TextArgs = nullptr;

    public:
        TextBlock();

        void Measure() override;

        void Draw(IDrawingContext * dc) override;
    };
} }
