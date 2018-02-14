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
#include "StationObject.h"
#include "ObjectJsonHelpers.h"

void StationObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());
    BaseImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void StationObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(BaseImageId, GetImageTable().GetCount());

    NameStringId = 0;
    BaseImageId = 0;
}

void StationObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = (height / 2) + 16;

    auto colour1 = COLOUR_LIGHT_BLUE;
    auto colour2 = COLOUR_BORDEAUX_RED;
    uint32 imageId = BaseImageId | (colour1 << 19) | (colour2 << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;

    gfx_draw_sprite(dpi, imageId + 4, x, y, 0);
    gfx_draw_sprite(dpi, imageId + 0, x, y, 0);
}

void StationObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    // auto properties = json_object_get(root, "properties");

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
