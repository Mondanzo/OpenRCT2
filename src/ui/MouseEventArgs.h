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

namespace OpenRCT2::Ui
{
    namespace MOUSE_BUTTON
    {
        constexpr sint8 LEFT    = 0;
        constexpr sint8 RIGHT   = 1;
    };

    struct MouseEventArgs
    {
        sint32  X;
        sint32  Y;
        sint8   Button;

        MouseEventArgs CopyAndOffset(sint32 x, sint32 y) const
        {
            auto result = *this;
            result.X += x;
            result.Y += y;
            return result;
        }
    };
}
