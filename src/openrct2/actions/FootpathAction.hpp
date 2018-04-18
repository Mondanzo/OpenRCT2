#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../OpenRCT2.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "GameAction.h"

enum class FOOTPATH_ACTION_TYPE
{
    CONSTRUCT,
    CONSTRUCT_FROM_TRACK,
    REMOVE,
};

namespace FOOTPATH_ACTION_FLAGS
{
    constexpr uint8 CLEAR_WALLS = 1 << 0;
    constexpr uint8 IS_QUEUE = 1 << 1;
};

/**
 * An action for constructing or removing footpath elements.
 */
struct FootpathAction : public GameActionBase<GAME_COMMAND_PLACE_PATH, GameActionResult>
{
private:
    uint8 _type;
    uint8 _entryIndex;
    uint8 _itemEntryIndex;
    CoordsXYZD _position;
    sint32 _slope;
    uint8 _flags;

public:
    FootpathAction() {}
    FootpathAction(FOOTPATH_ACTION_TYPE type, uint8 entryIndex, uint8 itemEntryIndex, CoordsXYZD position, sint32 slope, uint8 flags)
        : _type((uint8)type),
          _entryIndex(entryIndex),
          _itemEntryIndex(itemEntryIndex),
          _position(position),
          _slope(slope),
          _flags(flags)
    {
    }

    uint16 GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);
        stream << _type;
    }

    GameActionResult::Ptr Query() const override
    {
        auto result = CreateResult();
        if (_position.x >= gMapSizeUnits || _position.y >= gMapSizeUnits)
        {
            result->Error = GA_ERROR::DISALLOWED;
            result->ErrorMessage = STR_OFF_EDGE_OF_MAP;
            return result;
        }
        if (!map_can_build_at(_position.x, _position.y, _position.z))
        {
            result->Error = GA_ERROR::NOT_OWNED;
            result->ErrorMessage = STR_LAND_NOT_OWNED_BY_PARK;
            return result;
        }
        if (_slope & SLOPE_IS_IRREGULAR_FLAG)
        {
            result->Error = GA_ERROR::INVALID_PARAMETERS;
            result->ErrorMessage = STR_LAND_SLOPE_UNSUITABLE;
            return result;
        }
        if (_position.z < 16) {
            result->Error = GA_ERROR::TOO_LOW;
            result->ErrorMessage = STR_TOO_LOW;
            return result;
        }
        if (_position.z > 1984)
        {
            result->Error = GA_ERROR::TOO_HIGH;
            result->ErrorMessage = STR_TOO_HIGH;
            return result;
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto result = CreateResult();

        if (!IsPlacingGhost())
        {
            footpath_interrupt_peeps(_position.x, _position.y, _position.z);
            if ((_flags & FOOTPATH_ACTION_FLAGS::CLEAR_WALLS) && !gCheatsDisableClearanceChecks)
            {
                RemoveIntersectingWalls();
            }
        }

        // Force ride construction to recheck area
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_RECHECK;
        gFootpathPrice = 0;
        gFootpathGroundFlags = 0;

        footpath_provisional_remove();
        auto tileElement = map_get_footpath_element_slope(_position.x / 32, _position.y / 32, _position.z / 8, _slope);
        if (tileElement == nullptr)
        {
            return InsertElement(true);
        }
        else
        {
            return UpdateElement(tileElement, true);
        }
    }

private:
    static constexpr const uint8 byte_98D7EC[] = {
        207, 159, 63, 111
    };

    GameActionResult::Ptr CreateResult() const
    {
        auto result = MakeResult();
        result->ErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
        result->ExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;

        CoordsXYZ pos;
        pos.x = _position.x + 16;
        pos.y = _position.y + 16;
        pos.z = _position.z;
        result->Position = pos;

        return result;
    }

    void RemoveIntersectingWalls() const
    {
        // It is possible, let's remove walls between the old and new piece of path
        auto direction = _position.direction & 0xF;
        wall_remove_intersecting_walls(
            _position.x,
            _position.y,
            _position.z / 8,
            (_position.z / 8) + 4 + ((_slope & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK) ? 2 : 0),
            direction ^ 2);
        wall_remove_intersecting_walls(
            _position.x - TileDirectionDelta[direction].x,
            _position.y - TileDirectionDelta[direction].y,
            _position.z / 8,
            (_position.z / 8) + 4,
            direction);
    }

    GameActionResult::Ptr InsertElement(bool isExecuting) const
    {
        money32 price = 0;
        sint32 groundFlags = 0;

        sint32 bl, zHigh;

        if (!map_check_free_elements_and_reorganise(1))
        {
            auto result = CreateResult();
            result->Error = GA_ERROR::NO_FREE_ELEMENTS;
            result->ErrorMessage = STR_ERR_LANDSCAPE_DATA_AREA_FULL;
            return result;
        }

        auto flags = GetFlags();
        if (isExecuting && !IsPlacingGhost() && !(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
        {
            footpath_remove_litter(_position.x, _position.y, _position.z);
        }

        price += MONEY(12, 00);

        bl = 15;
        zHigh = (_position.z / 8) + 4;
        if (_slope & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED)
        {
            bl = byte_98D7EC[_slope & TILE_ELEMENT_DIRECTION_MASK];
            zHigh += 2;
        }

        auto entrancePath = false;
        auto entranceIsSamePath = false;
        auto entranceElement = map_get_park_entrance_element_at(_position.x, _position.y, _position.z / 8, false);
        // Make sure the entrance part is the middle
        if (entranceElement != nullptr && (entranceElement->properties.entrance.index & 0x0F) == 0)
        {
            entrancePath = true;
            // Make the price the same as replacing a path
            if (entranceElement->properties.entrance.path_type == _entryIndex)
            {
                entranceIsSamePath = true;
            }
            else
            {
                price -= MONEY(6, 00);
            }
        }

        // Clear anything that is in the way
        if (!entrancePath && !gCheatsDisableClearanceChecks)
        {
            // Allow building through a track unless building a queue or the footpath is sloped.
            uint8 crossingMode = CREATE_CROSSING_MODE_NONE;
            if (!IsPlacingQueue() && (_slope == TILE_ELEMENT_SLOPE_FLAT))
            {
                crossingMode = CREATE_CROSSING_MODE_PATH_OVER_TRACK;
            }
            bool clearResult = map_can_construct_with_clear_at(
                _position.x,
                _position.y,
                _position.z / 8,
                zHigh,
                &map_place_non_scenery_clear_func,
                bl,
                flags,
                &price,
                crossingMode);
            if (!clearResult)
            {
                auto result = CreateResult();
                result->Error = GA_ERROR::NO_CLEARANCE;
                result->ErrorMessage = gGameCommandErrorText;
                return result;
            }

            // Do not allow building footpath underwater
            groundFlags = gMapGroundFlags;
            if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
            {
                auto result = CreateResult();
                result->Error = GA_ERROR::NO_CLEARANCE;
                result->ErrorMessage = STR_CANT_BUILD_THIS_UNDERWATER;
                return result;
            }
        }

        // Add price of supports
        auto tileElement = map_get_surface_element_at(_position.xy());
        sint32 supportHeight = (_position.z / 8) - tileElement->base_height;
        if (supportHeight < 0)
        {
            price += MONEY(20, 00);
        }
        else
        {
            price += MONEY(5, 00) *  (supportHeight / 2);
        }

        if (isExecuting)
        {
            if (entrancePath)
            {
                if (!IsPlacingGhost() && !entranceIsSamePath)
                {
                    // Set the path type but make sure it's not a queue as that will not show up
                    entranceElement->properties.entrance.path_type = _entryIndex;
                    map_invalidate_tile_full(_position.x, _position.y);
                }
            }
            else
            {
                tileElement = tile_element_insert(_position.x / 32, _position.y / 32, _position.z / 8, 0x0F);
                if (tileElement == nullptr)
                {
                    auto result = CreateResult();
                    result->Error = GA_ERROR::NO_FREE_ELEMENTS;
                    result->ErrorMessage = STR_ERR_LANDSCAPE_DATA_AREA_FULL;
                    return result;
                }

                tileElement->type = TILE_ELEMENT_TYPE_PATH;
                tileElement->clearance_height = (_position.z / 8) + 4 + ((_slope & TILE_ELEMENT_SLOPE_NE_SIDE_UP) ? 2 : 0);
                footpath_element_set_type(tileElement, _entryIndex);
                tileElement->properties.path.type |= (_slope & TILE_ELEMENT_SLOPE_W_CORNER_DN);
                footpath_element_set_queue(tileElement, IsPlacingQueue());
                tileElement->properties.path.additions = _itemEntryIndex;
                tileElement->properties.path.addition_status = 255;
                tileElement->flags &= ~TILE_ELEMENT_FLAG_BROKEN;
                if (IsPlacingGhost())
                {
                    tileElement->flags |= TILE_ELEMENT_FLAG_GHOST;
                }

                footpath_queue_chain_reset();

                if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
                {
                    footpath_remove_edges_at(_position.x, _position.y, tileElement);
                }

                if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !IsPlacingGhost())
                {
                    MakeGuestSpawnTile({ _position.x, _position.y, tileElement->base_height * 8 });
                }

                FinaliseFootpathElement(_position.xy(), tileElement);
            }
        }

        // Prevent the place sound from being spammed
        if (entranceIsSamePath)
        {
            price = 0;
        }

        auto result = CreateResult();
        result->Cost = price;
        return result;
    }

    GameActionResult::Ptr UpdateElement(rct_tile_element * tileElement, bool isExecuting) const
    {
        auto result = CreateResult();
        money32 price = 0;
        if (footpath_element_get_type(tileElement) != _entryIndex ||
            footpath_element_is_queue(tileElement) != IsPlacingQueue())
        {
            price += MONEY(6, 00);
        }
        else if (_itemEntryIndex != 0)
        {
            auto flags = GetFlags();
            if (!IsPlacingGhost() &&
                footpath_element_get_path_scenery(tileElement) == _itemEntryIndex &&
                !(tileElement->flags & TILE_ELEMENT_FLAG_BROKEN))
            {
                if (flags & GAME_COMMAND_FLAG_4)
                {
                    result->Error = GA_ERROR::DISALLOWED;
                }
                else
                {
                    result->Cost = price;
                }
                return result;
            }

            auto pathItemEntry = get_footpath_item_entry(_itemEntryIndex - 1);
            auto pathItemEntryFlags = pathItemEntry->path_bit.flags;

            price += pathItemEntry->path_bit.price;

            if ((pathItemEntryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE) && footpath_element_is_sloped(tileElement))
            {
                result->Error = GA_ERROR::DISALLOWED;
                result->ErrorMessage = STR_CANT_BUILD_THIS_ON_SLOPED_FOOTPATH;
                return result;
            }

            if ((pathItemEntryFlags & PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE) && footpath_element_is_queue(tileElement))
            {
                result->Error = GA_ERROR::DISALLOWED;
                result->ErrorMessage = STR_CANNOT_PLACE_THESE_ON_QUEUE_LINE_AREA;
                return result;
            }

            if (!(pathItemEntryFlags & (PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER | PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)) &&
                (tileElement->properties.path.edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK) == 0x0F)
            {
                result->Error = GA_ERROR::DISALLOWED;
                return result;
            }

            if ((pathItemEntryFlags & PATH_BIT_FLAG_IS_QUEUE_SCREEN) && !footpath_element_is_queue(tileElement))
            {
                result->Error = GA_ERROR::DISALLOWED;
                result->ErrorMessage = STR_CAN_ONLY_PLACE_THESE_ON_QUEUE_AREA;
                return result;
            }

            if (flags & GAME_COMMAND_FLAG_4)
            {
                result->Error = GA_ERROR::DISALLOWED;
                return result;
            }

            if (IsPlacingGhost())
            {
                if (footpath_element_has_path_scenery(tileElement))
                {
                    // There is already an item on the footpath, therefore we can't set it to the ghost of
                    // our new item.
                    result->Error = GA_ERROR::DISALLOWED;
                    return result;
                }

                // There is nothing yet - check if we should place a ghost
                if (isExecuting)
                {
                    footpath_scenery_set_is_ghost(tileElement, true);
                }
            }
            else
            {
                if (isExecuting)
                {
                    footpath_scenery_set_is_ghost(tileElement, false);
                }
            }

            if (isExecuting)
            {
                footpath_element_set_path_scenery(tileElement, _itemEntryIndex);
                tileElement->flags &= ~TILE_ELEMENT_FLAG_BROKEN;
                if (_itemEntryIndex != 0)
                {
                    if (pathItemEntryFlags & PATH_BIT_FLAG_IS_BIN)
                    {
                        tileElement->properties.path.addition_status = 255;
                    }
                }
                map_invalidate_tile_full(_position.x, _position.y);
            }

            result->Cost = price;
            return result;
        }

        if (GetFlags() & GAME_COMMAND_FLAG_4)
        {
            result->Error = GA_ERROR::DISALLOWED;
            return result;
        }

        if (isExecuting)
        {
            footpath_queue_chain_reset();
            if (!(GetFlags() & GAME_COMMAND_FLAG_PATH_SCENERY))
            {
                footpath_remove_edges_at(_position.x, _position.y, tileElement);
            }
            footpath_element_set_type(tileElement, _entryIndex);
            footpath_element_set_queue(tileElement, IsPlacingQueue());
            footpath_element_set_path_scenery(tileElement, _itemEntryIndex);
            tileElement->flags &= ~TILE_ELEMENT_FLAG_BROKEN;

            FinaliseFootpathElement(_position.xy(), tileElement);
        }

        result->Cost = price;
        return result;
    }

    void FinaliseFootpathElement(CoordsXY position, rct_tile_element * tileElement) const
    {
        if (footpath_element_is_sloped(tileElement) && !IsPlacingGhost())
        {
            auto direction = footpath_element_get_slope_direction(tileElement);
            auto z = tileElement->base_height;
            wall_remove_intersecting_walls(position.x, position.y, z, z + 6, direction ^ 2);
            wall_remove_intersecting_walls(position.x, position.y, z, z + 6, direction);
            // Removing walls may have made the pointer invalid, so find it again
            tileElement = map_get_footpath_element(position.x / 32, position.y / 32, z);
        }

        auto flags = GetFlags();
        if (!(flags & GAME_COMMAND_FLAG_PATH_SCENERY))
        {
            footpath_connect_edges(position.x, position.y, tileElement, flags);
        }

        footpath_update_queue_chains();
        map_invalidate_tile_full(position.x, position.y);
    }

    void MakeGuestSpawnTile(CoordsXYZ location) const
    {
        uint8 direction = 0;
        if (location.x != 32)
        {
            direction++;
            if (location.y != gMapSizeUnits - 32)
            {
                direction++;
                if (location.x != gMapSizeUnits - 32)
                {
                    direction++;
                    if (location.y != 32)
                    {
                        return;
                    }
                }
            }
        }

        auto peepSpawn = &gPeepSpawns[0];
        peepSpawn->x = location.x + (word_981D6C[direction].x * 15) + 16;
        peepSpawn->y = location.y + (word_981D6C[direction].y * 15) + 16;
        peepSpawn->direction = direction;
        peepSpawn->z = location.z;
    }

    bool IsPlacingGhost() const
    {
        return GetFlags() & GAME_COMMAND_FLAG_GHOST;
    }

    bool IsPlacingQueue() const
    {
        return _flags & FOOTPATH_ACTION_FLAGS::IS_QUEUE;
    }
};
