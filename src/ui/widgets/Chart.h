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

#include <string>
#include "../Widget.h"

namespace OpenRCT2::Ui
{
    class Chart : public Widget
    {
    private:
        const uint8 *   _values = nullptr;
        size_t          _valuesCount = 0;

    public:
        void SetValues(const uint8 * values, size_t count);

        void Draw(IDrawingContext * dc) override;
    };
}
