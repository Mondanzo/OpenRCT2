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

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../world/Footpath.h"
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
};

/**
 * An action for constructing or removing footpath elements.
 */
struct FootpathAction : public GameActionBase<GAME_COMMAND_PLACE_PATH, GameActionResult>
{
private:
    FOOTPATH_ACTION_TYPE _type;
    uint8 _entryIndex;
    uint8 _itemEntryIndex;
    CoordsXYZD _position;
    sint32 _slope;
    uint8 _flags;

public:
    FootpathAction() {}
    FootpathAction(FOOTPATH_ACTION_TYPE type, uint8 entryIndex, uint8 itemEntryIndex, CoordsXYZD position, sint32 slope, uint8 flags)
        : _type(type),
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

        auto flags = GetFlags();
        if (!(flags & GAME_COMMAND_FLAG_GHOST))
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
            return footpath_element_insert(_entryIndex, _position.x, _position.y, _position.z / 8, _slope, flags, _itemEntryIndex);
        }
        else
        {
            return footpath_element_update(_position.x, _position.y, tileElement, _entryIndex, flags, _itemEntryIndex);
        }
    }

private:
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
};
