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

#include "../../drawing/DrawingContext.h"
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

        rect32 renderBounds = GetViewportRenderBounds();
        rct_window dummy;
        viewport_create(
            &dummy,
            renderBounds.X, renderBounds.Y, renderBounds.Width, renderBounds.Height,
            _zoom,
            _location.X,
            _location.Y,
            _location.Z,
            0,
            -1);
        _viewport = dummy.viewport;
    }
    else
    {
        rect32 renderBounds = GetViewportRenderBounds();

        _viewport->x = renderBounds.X;
        _viewport->y = renderBounds.Y;
        _viewport->width = renderBounds.Width;
        _viewport->height = renderBounds.Height;
        _viewport->view_width = renderBounds.Width << _zoom;
        _viewport->view_height = renderBounds.Height << _zoom;
        _viewport->zoom = _zoom;

        xyz32 location = _location;
        if (_trackFlags & VIEWPORT_TRACK_FLAGS::TRACK_SPRITE)
        {
            rct_sprite * sprite = get_sprite(_spriteIndex);
            location.X = sprite->unknown.x;
            location.Y = sprite->unknown.y;
            location.Z = sprite->unknown.z;
        }

        xy32 viewPosition;
        center_2d_coordinates(location.X, location.Y, location.Z, &viewPosition.X, &viewPosition.Y, _viewport);
        _viewport->view_x = viewPosition.X;
        _viewport->view_y = viewPosition.Y;
    }
}

void ViewportWidget::Draw(IDrawingContext * dc)
{
    const WindowStyle * style = ParentWindow->GetStyle();
    colour_t colour = style->GetColour(Style);
    dc->FillRect3D(0, 0, Width - 1, Height - 1, colour, INSET_RECT_F_60);

    if (_viewport != nullptr)
    {
        rect32 renderBounds = GetViewportRenderBounds();

        uintptr_t dpip = ((uintptr_t *)dc)[2];
        rct_drawpixelinfo * dpi = (rct_drawpixelinfo *)dpip;
        viewport_render(dpi, _viewport, renderBounds.X, renderBounds.Y, renderBounds.GetRight(), renderBounds.GetBottom());
    }
}

rect32 ViewportWidget::GetViewportRenderBounds()
{
    rect32 renderBounds;
    renderBounds.X = 1;
    renderBounds.Y = 1;
    renderBounds.Width = Width - 2;
    renderBounds.Height = Height - 2;
    return renderBounds;
}
