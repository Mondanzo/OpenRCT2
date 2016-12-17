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
#include "../../sprites.h"
#include "../DrawingContextExtensions.h"
#include "../MouseEventArgs.h"
#include "../TabImages.h"
#include "TabPanel.h"

using namespace OpenRCT2::Ui;

constexpr sint32 TAB_WIDTH = 31;
constexpr sint32 TAB_HEIGHT = 27;

TabPanel::TabPanel()
{
}

TabPanel::~TabPanel()
{
}

sint32 TabPanel::GetTabCount()
{
    return (sint32)_tabs.size();
}

void TabPanel::SetAdapter(ITabPanelAdapter * adapter)
{
    _adapter = adapter;
    _dirty = true;
}

void TabPanel::SetSelectedIndex(sint32 index)
{
    if (_selectedIndex != index)
    {
        _selectedIndex = index;
        SetupWidgets();
    }
}

void TabPanel::Invalidate()
{
    _dirty = true;
}

sint32 TabPanel::GetChildrenCount()
{
    return GetTabCount() + 1;
}

Widget * TabPanel::GetChild(sint32 index)
{
    sint32 numTabs = GetTabCount();
    if (index >= 0 && index < numTabs)
    {
        return &_tabs[index];
    }
    else if (index == numTabs)
    {
        return &_container;
    }
    return nullptr;
}

void TabPanel::Measure()
{
    size32 size;
    size.Width = 1 + (GetTabCount() * TAB_WIDTH);
    size.Height = TAB_HEIGHT;

    Widget * content = _container.GetChild();
    if (content != nullptr)
    {
        size.Width += content->Width;
        size.Height += content->Height;
    }

    Size = size;
}

void TabPanel::Arrange()
{
    _container.X = 1;
    _container.Y = TAB_HEIGHT;
    _container.Width = Width - _container.X - 1;
    _container.Height = Height - _container.Y - 1;

    sint32 x = 3;
    for (Tab &tab : _tabs)
    {
        tab.X = x;
        tab.Y = 0;
        x += TAB_WIDTH;
    }
}

void TabPanel::Update()
{
    if (_dirty)
    {
        _dirty = false;
        SetupWidgets();
    }
}

void TabPanel::Draw(IDrawingContext * dc)
{
    DCExtensions::FillRectInset(dc, 0, 26, Width - 1, Height - 1, COLOUR_DARK_YELLOW, 0);
}

void TabPanel::SetupWidgets()
{
    sint32 numTabs = 0;
    if (_adapter != nullptr)
    {
        numTabs = _adapter->GetTabCount();
    }
    _tabs.resize(numTabs);

    if (numTabs == 0)
    {
        _selectedIndex = -1;
        return;
    }
    if (_selectedIndex == -1 || _selectedIndex >= numTabs)
    {
        _selectedIndex = 0;
    }

    for (sint32 i = 0; i < numTabs; i++)
    {
        auto tab = &_tabs[i];
        tab->Width = TAB_WIDTH;
        tab->Height = TAB_HEIGHT;
        tab->Info = *(_adapter->GetTabInfo(i));
        if (i == _selectedIndex)
        {
            tab->Active = true;
        }
    }

    Widget * content = _adapter->GetContent(_selectedIndex);
    if (content != nullptr)
    {
        _container.SetChild(content);
    }

    InvalidateLayout();
}

void TabPanel::Tab::Update()
{
    if (Active)
    {
        if (FrameTimeout > 0)
        {
            FrameTimeout--;
        }
        else
        {
            const TabImage * ti = Info.Image;
            FrameTimeout = ti->FrameDuration - 1;
            Offset++;
            if (Offset >= ti->FrameCount && ti->FrameCount != 0)
            {
                Offset = 0;
            }
            InvalidateVisual();
        }
    }
}

void TabPanel::Tab::Draw(IDrawingContext * dc)
{
    uint32 colour = COLOUR_DARK_YELLOW;
    uint32 sprite = SPR_TAB;
    if (Active)
    {
        sprite = SPR_TAB_ACTIVE;
    }
    sprite |= 0x20000000;
    sprite |= colour << 19;
    dc->DrawSprite(sprite, 0, 0, 0);

    const TabImage * ti = Info.Image;
    if (ti != nullptr)
    {
        if (ti->FrameCount > 0)
        {
            sprite = ti->StartFrame;
            if (Active)
            {
                sprite += Offset;
            }
            dc->DrawSprite(sprite, 0, 0, 0);
        }

        auto handler = ti->DrawHandler;
        if (handler != nullptr)
        {
            handler(dc, Offset);
        }
    }
}
