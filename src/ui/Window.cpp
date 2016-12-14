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

#include "../drawing/IDrawingContext.h"
#include "widgets/Button.h"
#include "Window.h"

using namespace OpenRCT2::Ui;

Window::Window()
{
    _child = new Button();
    _child->X = 12;
    _child->Y = 12;
    _child->Width = 15;
    _child->Height = 15;
}

void Window::Update()
{
}

void Window::Draw(IDrawingContext * dc)
{
    uint32 colour = 72;
    if (Flags & WINDOW_FLAGS::FOCUS)
    {
        colour = 152;
    }
    dc->FillRect(colour, 0, 0, Width, Height);

    dc->Nest(_child->X, _child->Y, _child->Width, _child->Height);
    _child->Draw(dc);
}

bool Window::HitTest(sint32 x, sint32 y)
{
    bool isTransparent = (Flags & WINDOW_FLAGS::TRANSPARENT) != 0;
    return isTransparent;
}

void Window::MouseDown(const MouseEventArgs * e)
{
}

void Window::MouseMove(const MouseEventArgs * e)
{
}

void Window::MouseUp(const MouseEventArgs * e)
{
}

void Window::MouseWheel(const MouseEventArgs * e)
{
}
