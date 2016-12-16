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

#include "../drawing/IDrawingContext.h"
#include "../sprites.h"
#include "TabImages.h"

extern "C"
{
    #include "../world/sprite.h"
}

using namespace OpenRCT2::Ui;

TabImage TabImage::Create(uint32 base)
{
    TabImage result;
    result.StartFrame = base;
    result.FrameCount = 1;
    result.FrameDuration = 0;
    result.DrawHandler = nullptr;
    return result;
}

TabImage TabImage::Create(uint32 base, uint32 count, uint32 duration)
{
    TabImage result;
    result.StartFrame = base;
    result.FrameCount = count;
    result.FrameDuration = duration;
    result.DrawHandler = nullptr;
    return result;
}

TabImage TabImage::Create(uint32 base, uint32 count, uint32 duration, TabImageDrawAction handler)
{
    TabImage result;
    result.StartFrame = base;
    result.FrameCount = count;
    result.FrameDuration = duration;
    result.DrawHandler = handler;
    return result;
}

namespace OpenRCT2::Ui
{
    namespace TabImages
    {
        const TabImage Park = TabImage::Create(SPR_TAB_PARK);
        const TabImage Admission = TabImage::Create(SPR_TAB_ADMISSION_0, 8, 2);
        const TabImage Statistics = TabImage::Create(SPR_TAB_STATS_0, 7, 2);
        const TabImage Objective = TabImage::Create(SPR_TAB_OBJECTIVE_0, 16, 2);
        const TabImage Awards = TabImage::Create(SPR_TAB_AWARDS);

        const TabImage RatingChart = TabImage::Create(SPR_TAB_GRAPH_0, 8, 5, [](IDrawingContext * dc, sint32 offset) -> void
        {
            dc->DrawSprite(SPR_RATING_HIGH, 7, 1, 0);
            dc->DrawSprite(SPR_RATING_LOW, 16, 12, 0);
        });

        const TabImage GuestChart = TabImage::Create(0, 0, 1, [](IDrawingContext * dc, sint32 offset) -> void
        {
            uint32 sprite = SPR_TAB_GRAPH_0;
            uint32 frame = (offset / 8) % 8;
            sprite += frame;
            dc->DrawSprite(sprite, 0, 0, 0);

            sprite = g_sprite_entries[PEEP_SPRITE_TYPE_NORMAL].sprite_image->base_image + 1;
            frame = (offset / 4) % 6;
            sprite += frame * 4;
            sprite |= 0xA9E00000;
            dc->DrawSprite(sprite, 15, 17, 0);
        });
    }
}
