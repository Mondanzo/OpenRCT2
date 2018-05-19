#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <stdexcept>
#include "Object.h"

enum TERRAIN_SURFACE_FLAGS
{
    NONE                = 0,
    SMOOTH_WITH_SELF    = 1 << 0,
    SMOOTH_WITH_OTHER   = 1 << 1,
};

class TerrainSurfaceObject final : public Object
{
private:

public:
    rct_string_id NameStringId{};
    uint32 IconImageId{};
    uint32 BaseImageId{};
    uint32 GridBaseImageId{};
    uint32 UndergroundBaseImageId{};
    uint32 PatternBaseImageId{};
    colour_t Colour{};
    uint8 Rotations{};
    money32 Price{};
    TERRAIN_SURFACE_FLAGS Flags{};

    explicit TerrainSurfaceObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData() override { return nullptr; }
    void ReadLegacy(IReadObjectContext * context, IStream * stream) override { throw std::runtime_error("Not supported."); }

    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const override;
};
