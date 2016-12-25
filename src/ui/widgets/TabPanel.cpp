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

#include "../../core/Math.hpp"
#include "../../drawing/DrawingContext.h"
#include "../../sprites.h"
#include "../MouseEventArgs.h"
#include "../TabImages.h"
#include "../Window.h"
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
    SetupWidgets();
}

sint32 TabPanel::GetSelectedIndex()
{
    return _selectedIndex;
}

void TabPanel::SetSelectedIndex(sint32 index)
{
    if (_selectedIndex != index)
    {
        _selectedIndex = index;
        SetupWidgets();
    }
}

void TabPanel::Refresh()
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
    // TODO don't include hidden tabs
    size.Width = 3 + (GetTabCount() * TAB_WIDTH) + 3;
    size.Height = TAB_HEIGHT + 1;

    size32 containerSize = _container.GetSizeWithMargin();
    size.Width = Math::Max(size.Width, containerSize.Width);
    size.Height += containerSize.Height;

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
        if (tab.Info.Flags & TABINFO_FLAGS::HIDDEN)
        {
            tab.Visibility = VISIBILITY::COLLAPSED;
        }
        else
        {
            tab.Visibility = VISIBILITY::VISIBLE;
            tab.X = x;
            tab.Y = 0;
            x += TAB_WIDTH;
        }
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
    colour_t colour = ParentWindow->Style.GetColour(Style);
    dc->FillRect3D(0, 26, Width - 1, Height - 1, colour, 0);
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
        tab->Parent = this;
        tab->Index = i;
        tab->Width = TAB_WIDTH;
        tab->Height = TAB_HEIGHT;
        tab->Info = _adapter->GetTabInfo(i);
        tab->Active = (i == _selectedIndex);
    }

    Widget * content = _adapter->GetContent(_selectedIndex);
    _container.SetChild(content);

    InvalidateLayout();
}

TabPanel::Tab::Tab()
{
    Type = BUTTON_TYPE::IMAGE;
    Image = SPR_TAB;
    ImageDown = SPR_TAB_ACTIVE;

    FrameTimeout = 0;
    Offset = 0;

    ClickEvent = std::bind(&TabPanel::Tab::ClickHandler, this);
}

void TabPanel::Tab::Update()
{
    Button::Update();

    if (Active)
    {
        ButtonFlags |= BUTTON_FLAGS::DOWN;
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
    else
    {
        ButtonFlags &= ~BUTTON_FLAGS::DOWN;
        FrameTimeout = 0;
        Offset = 0;
    }
}

void TabPanel::Tab::Draw(IDrawingContext * dc)
{
    Button::Draw(dc);

    const TabImage * ti = Info.Image;
    if (ti != nullptr)
    {
        if (ti->FrameCount > 0)
        {
            uint32 sprite = ti->StartFrame;
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

void TabPanel::Tab::ClickHandler()
{
    Parent->SetSelectedIndex(Index);
}
