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

interface IDrawingContext;

namespace OpenRCT2 { namespace Ui
{
    struct MouseEventArgs;

    namespace WINDOW_FLAGS
    {
        constexpr uint32 FOCUS           = 1 << 0;
        constexpr uint32 CURSOR          = 1 << 1;
        constexpr uint32 MODAL           = 1 << 2;
        constexpr uint32 STICK_TO_BACK   = 1 << 3;
        constexpr uint32 STICK_TO_FRONT  = 1 << 4;
        constexpr uint32 TRANSPARENT     = 1 << 5;
    }

    class Window
    {
    public:
        union
        {
            struct { sint32 X, Y, Width, Height; };
            struct { xy32 Location; size32 Size; };
            rect32 Bounds;
        };
        uint32 Flags;

    public:
        virtual ~Window() = default;

        virtual void Update();
        virtual void Draw(IDrawingContext * g);

        virtual bool HitTest(sint32 x, sint32 y);
        virtual void MouseDown(const MouseEventArgs * e);
        virtual void MouseMove(const MouseEventArgs * e);
        virtual void MouseUp(const MouseEventArgs * e);
        virtual void MouseWheel(const MouseEventArgs * e);
    };
} }
