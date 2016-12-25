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

extern "C"
{
    #include "drawing.h"
}

namespace STRING_FLAGS
{
    constexpr uint32 HALIGN_LEFT        = 0b0000;
    constexpr uint32 HALIGN_MIDDLE      = 0b0001;
    constexpr uint32 HALIGN_RIGHT       = 0b0011;
    constexpr uint32 VALIGN_TOP         = 0b0100;
    constexpr uint32 VALIGN_MIDDLE      = 0b1000;
    constexpr uint32 VALIGN_BOTTOM      = 0b1100;

    constexpr uint32 CLIPPED            = 1 << 4;
    constexpr uint32 WRAPPED            = 1 << 5;
};

interface IDrawingEngine;

interface IDrawingContext
{
    virtual ~IDrawingContext() { }

    virtual IDrawingEngine * GetEngine() abstract;

    virtual IDrawingContext * Nest(sint32 x, sint32 y, sint32 width, sint32 height) abstract;

    virtual void Clear(uint8 paletteIndex)                                                                abstract;
    virtual void FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)            abstract;
    virtual void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) abstract;
    virtual void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)                      abstract;
    virtual void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)                      abstract;
    virtual void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)            abstract;
    virtual void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)                          abstract;
    virtual void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette)                             abstract;

    // Extensions
    void DrawString(rct_string_id stringId, const void * args, sint32 x, sint32 y, uint32 colour, uint32 flags, sint32 width = 0);
    void DrawString(const utf8 * text, sint32 x, sint32 y, uint32 colour, uint32 flags, sint32 width = 0);
    void FillRect3D(sint32 left, sint32 top, sint32 right, sint32 bottom, uint32 colour, uint32 flags);
};
