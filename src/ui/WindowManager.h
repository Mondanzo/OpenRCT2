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
#include "Primitives.h"

interface   IDrawingContext;
struct      MouseEventArgs;

namespace OpenRCT2 { namespace Ui
{
    interface IWindowManager
    {
        virtual ~IWindowManager() = default;

        virtual rect32 GetBounds() const abstract;

        virtual void SetBounds(rect32 bounds) abstract;

        virtual void Update() abstract;
        virtual void Draw(IDrawingContext * dc) abstract;

        // Interaction
        virtual void MouseDown(const MouseEventArgs * e) abstract;
        virtual void MouseMove(const MouseEventArgs * e) abstract;
        virtual void MouseUp(const MouseEventArgs * e) abstract;
        virtual void MouseWheel(const MouseEventArgs * e) abstract;
    };

    IWindowManager * CreateWindowManager();
} }
