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

#pragma pack(push, 1)

struct xy32
{
    sint32 X, Y;
};

struct xyz32
{
    sint32 X, Y, Z;
};

struct size32
{
    sint32 Width, Height;
};

struct rect32
{
    union
    {
        struct
        {
            sint32 X, Y, Width, Height;
        };
        struct
        {
            xy32 XY;
            size32 Size;
        };
    };

    sint32 GetRight() { return X + Width; }
    sint32 GetBottom() { return Y + Height; }

    bool Contains(sint32 x, sint32 y)
    {
        return x >= X && y >= Y && x < GetRight() && y < GetBottom();
    }
};

#pragma pack(pop)
