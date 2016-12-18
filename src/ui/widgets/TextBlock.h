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

#include <string>
#include "../Widget.h"

namespace OpenRCT2::Ui
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
    private:
        std::string             _text;
        colour_t                _colour = 0;
        bool                    _wrap = false;
        sint32                  _lastMeasuredWidth;

    public:
        HORIZONTAL_ALIGNMENT    HorizontalAlignment;
        VERTICAL_ALIGNMENT      VerticalAlignment;

    public:
        TextBlock();

        std::string GetText();
        void SetText(const std::string &value);
        colour_t GetColour();
        void SetColour(colour_t value);
        void SetWrapping(bool value);

        void Measure() override;

        void Update() override;
        void Draw(IDrawingContext * dc) override;
    };
}
