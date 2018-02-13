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
#include "RideEntranceExitObject.h"
#include "ObjectJsonHelpers.h"

void RideEntranceExitObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());
    BaseImageId = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void RideEntranceExitObject::Unload()
{
    language_free_object_string(NameStringId);
    gfx_object_free_images(BaseImageId, GetImageTable().GetCount());

    NameStringId = 0;
    BaseImageId = 0;
}

void RideEntranceExitObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    sint32 x = width / 2;
    sint32 y = height / 2;

    uint32 imageId = BaseImageId;
    gfx_draw_sprite(dpi, imageId + 5, x + 8, y - 8, 0);
    gfx_draw_sprite(dpi, imageId + 5, x + 8, y + 8, 0);
}

void RideEntranceExitObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    // auto properties = json_object_get(root, "properties");

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}
