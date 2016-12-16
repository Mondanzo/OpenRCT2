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

interface IDrawingContext;

namespace OpenRCT2::Ui
{
    typedef void (* TabImageDrawAction)(IDrawingContext *, sint32 offset);

    struct TabImage
    {
        uint32  StartFrame;
        sint32  FrameCount;
        sint32  FrameDuration;
        TabImageDrawAction DrawHandler;

        static TabImage Create(uint32 base);
        static TabImage Create(uint32 base, uint32 count, uint32 duration);
        static TabImage Create(uint32 base, uint32 count, uint32 duration, TabImageDrawAction handler);
    };

    namespace TabImages
    {
        extern const TabImage Admission;
        extern const TabImage Awards;
        extern const TabImage GuestChart;
        extern const TabImage Objective;
        extern const TabImage Park;
        extern const TabImage RatingChart;
        extern const TabImage Statistics;
    }
}
