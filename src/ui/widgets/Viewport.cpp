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

#include "../../drawing/IDrawingContext.h"
#include "../DrawingContextExtensions.h"
#include "../Window.h"
#include "Viewport.h"

extern "C"
{
    #include "../../interface/viewport.h"
}

using namespace OpenRCT2::Ui;

ViewportWidget::ViewportWidget()
{

}

ViewportWidget::~ViewportWidget()
{
    if (_viewport != nullptr)
    {
        _viewport->width = 0;
    }
}

rct_viewport * ViewportWidget::GetViewport()
{
    return _viewport;
}

void ViewportWidget::SetLocation(xyz32 location)
{
    _location = location;
    _trackFlags |= VIEWPORT_TRACK_FLAGS::INITIALISED;
    _trackFlags &= ~VIEWPORT_TRACK_FLAGS::TRACK_SPRITE;
}

void ViewportWidget::SetSpriteIndex(uint16 sprite)
{
    _spriteIndex = sprite;
    _trackFlags |= VIEWPORT_TRACK_FLAGS::INITIALISED;
    _trackFlags |= VIEWPORT_TRACK_FLAGS::TRACK_SPRITE;
}

void ViewportWidget::SetZoom(uint8 zoom)
{
    _zoom = zoom;
}

void ViewportWidget::Update()
{
    if (_viewport == nullptr)
    {
        if (Width < 8 || Height < 8)
        {
            return;
        }

        rect32 absoluteBounds = GetAbsoluteBounds();
        rct_window dummy;
        viewport_create(
            &dummy,
            absoluteBounds.X, absoluteBounds.Y, absoluteBounds.Width, absoluteBounds.Height,
            _zoom,
            _location.X,
            _location.Y,
            _location.Z,
            0,
            -1);
        _viewport = dummy.viewport;
    }
}

void ViewportWidget::Draw(IDrawingContext * dc)
{
    colour_t colour = ParentWindow->Style.GetColour(Style);
    DCExtensions::FillRectInset(dc, 0, 0, Width - 1, Height - 1, colour, INSET_RECT_F_60);

    if (_viewport != nullptr)
    {
        rect32 absoluteBounds = GetAbsoluteBounds();

        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        viewport_render(dpi, _viewport, absoluteBounds.X, absoluteBounds.Y, absoluteBounds.GetRight() - 1, absoluteBounds.GetBottom() - 1);
    }
}

rect32 ViewportWidget::GetAbsoluteBounds()
{
    rect32 absoluteBounds;
    absoluteBounds.X = ParentWindow->X;
    absoluteBounds.Y = ParentWindow->Y;
    absoluteBounds.Width = Width - 2;
    absoluteBounds.Height = Height - 2;
    return absoluteBounds;
}
