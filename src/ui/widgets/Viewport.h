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

#include "../Widget.h"

struct rct_viewport;

namespace OpenRCT2::Ui
{
    namespace VIEWPORT_TRACK_FLAGS
    {
        constexpr uint8 INITIALISED     = 1 << 0;
        constexpr uint8 TRACK_SPRITE    = 1 << 1;
    }

    class ViewportWidget : public Widget
    {
    private:
        rct_viewport *  _viewport = nullptr;
        xyz32           _location = { 0 };
        uint16          _spriteIndex = 0xFFFF;
        uint8           _zoom = 0;
        uint8           _trackFlags = 0;

    public:
        ViewportWidget();
        ~ViewportWidget();

        rct_viewport * GetViewport();

        void SetLocation(xyz32 location);
        void SetSpriteIndex(uint16 sprite);
        void SetZoom(uint8 zoom);

        void Update() override;
        void Draw(IDrawingContext * dc) override;

    private:
        rect32 GetViewportRenderBounds();
    };
}
