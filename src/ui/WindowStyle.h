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

#include "../common.h"
#include "Primitives.h"

interface IDrawingContext;

namespace OpenRCT2::Ui
{
    namespace STYLE_FLAGS
    {
        constexpr uint8 TRAFFIC_LIGHTS  = 1 << 0;
        constexpr uint8 RCT1_FONTS      = 1 << 1;
    }

    struct WindowStyle
    {
        static constexpr int MAX_COLOURS = 6;

        uint8       Flags = 0;
        colour_t    Colours[MAX_COLOURS] = { 0 };

        bool     HasFlag(uint8 flag);
        colour_t GetColour(uint8 style);
    };
}
