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

#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "TerrainSurfaceObject.h"
#include "ObjectJsonHelpers.h"

void TerrainSurfaceObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());
    IconImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());

    // First image is icon followed by edge images
    BaseImageId = IconImageId + 1;
}

void TerrainSurfaceObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(IconImageId, GetImageTable().GetCount());

    NameStringId = 0;
    IconImageId = 0;
    BaseImageId = 0;
}

void TerrainSurfaceObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    uint32 imageId = BaseImageId;

    sint32 x0 = 0;
    sint32 y = -16;
    for (sint32 i = 0; i < 8; i++)
    {
        sint32 x = x0;
        if (i % 2 == 0)
        {
            x -= 32;
        }
        for (sint32 j = 0; j < 4; j++)
        {
            gfx_draw_sprite(dpi, imageId, x, y, 0);
            x += 64;
        }
        y += 16;
    }
}

void TerrainSurfaceObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");
    Price = ObjectJsonHelpers::GetInteger(properties, "price", 0);

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
