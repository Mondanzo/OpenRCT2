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

#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/Input.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Surface.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>

using namespace OpenRCT2;

// clang-format off
static uint16 toolSizeSpriteIndices[] =
{
    SPR_LAND_TOOL_SIZE_0,
    SPR_LAND_TOOL_SIZE_1,
    SPR_LAND_TOOL_SIZE_2,
    SPR_LAND_TOOL_SIZE_3,
    SPR_LAND_TOOL_SIZE_4,
    SPR_LAND_TOOL_SIZE_5,
    SPR_LAND_TOOL_SIZE_6,
    SPR_LAND_TOOL_SIZE_7,
};
// clang-format on

uint16 gLandToolSize;
money32 gLandToolRaiseCost;
money32 gLandToolLowerCost;
uint8 gLandToolTerrainSurface;
uint8 gLandToolTerrainEdge;
money32 gWaterToolRaiseCost;
money32 gWaterToolLowerCost;

uint32 land_tool_size_to_sprite_index(uint16 size)
{
    if (size <= MAX_TOOL_SIZE_WITH_SPRITE)
    {
        return toolSizeSpriteIndices[size];
    }
    else
    {
        return 0xFFFFFFFF;
    }
}

void land_tool_show_surface_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentSurfaceType)
{
    auto objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_SURFACE_OBJECTS; i++)
    {
        const auto surfaceObj = static_cast<TerrainSurfaceObject *>(objManager->GetLoadedObject(OBJECT_TYPE_TERRAIN_SURFACE, i));
        if (surfaceObj != nullptr)
        {
            gDropdownItemsFormat[itemIndex] = DROPDOWN_FORMAT_LAND_PICKER;
            gDropdownItemsArgs[itemIndex] = surfaceObj->IconImageId;
            if (i == currentSurfaceType)
            {
                defaultIndex = itemIndex;
            }
            itemIndex++;
        }
    }
    auto surfaceCount = itemIndex;

    window_dropdown_show_image(
       w->x + widget->left, w->y + widget->top,
       widget->bottom - widget->top,
       w->colours[2],
       0,
        surfaceCount,
       47, 36,
       gAppropriateImageDropdownItemsPerRow[surfaceCount]
    );

    gDropdownDefaultIndex = defaultIndex;
}

void land_tool_show_edge_style_dropdown(rct_window * w, rct_widget * widget, uint8 currentEdgeType)
{
    auto objManager = GetContext()->GetObjectManager();

    auto defaultIndex = 0;
    auto itemIndex = 0;
    for (size_t i = 0; i < MAX_TERRAIN_EDGE_OBJECTS; i++)
    {
        const auto edgeObj = static_cast<TerrainEdgeObject *>(objManager->GetLoadedObject(OBJECT_TYPE_TERRAIN_EDGE, i));
        if (edgeObj != nullptr)
        {
            gDropdownItemsFormat[itemIndex] = DROPDOWN_FORMAT_LAND_PICKER;
            gDropdownItemsArgs[itemIndex] = edgeObj->IconImageId;
            if (i == currentEdgeType)
            {
                defaultIndex = itemIndex;
            }
            itemIndex++;
        }
    }
    auto edgeCount = itemIndex;

    window_dropdown_show_image(
       w->x + widget->left, w->y + widget->top,
       widget->bottom - widget->top,
       w->colours[2],
       0,
       edgeCount,
       47, 36,
       gAppropriateImageDropdownItemsPerRow[edgeCount]
    );

    gDropdownDefaultIndex = defaultIndex;
}
