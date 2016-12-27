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

#include "../core/Math.hpp"
#include "../localisation/string_ids.h"
#include "widgets/Button.h"
#include "widgets/TitleBar.h"
#include "widgets/TabPanel.h"
#include "Window.h"
#include "WindowShell.h"

using namespace OpenRCT2::Ui;

WindowShell::WindowShell(Window * window)
{
    SetParentWindow(window);
}

WindowShell::~WindowShell()
{
    delete _titleBar;
    delete _closeButton;
    delete _tabPanel;
}

void WindowShell::Initialise()
{
    // Title bar
    if (GetParentWindow()->HasFlag(WINDOW_FLAGS::HAS_TITLE_BAR))
    {
        _titleBar = new TitleBar();
        _titleBar->SetText(GetParentWindow()->GetTitle());
        AddChild(_titleBar);
    }

    // Close button
    if (GetParentWindow()->HasFlag(WINDOW_FLAGS::HAS_TITLE_BAR))
    {
        _closeButton = new Button();
        _closeButton->Text = STR_CLOSE_X;
        _closeButton->Type = BUTTON_TYPE::OUTSET;
        AddChild(_closeButton);
    }

    // Tab panel
    _tabPanel = new TabPanel();
    _tabPanel->SetAdapter(GetParentWindow()->GetTabPanelAdapter());
    AddChild(_tabPanel);
}

void WindowShell::Measure()
{
    size32 size = { 0 };
    if (_titleBar != nullptr)
    {
        size.Width = 16;
        size.Height += _titleBar->GetHeight();
    }
    if (_tabPanel != nullptr)
    {
        size.Width = Math::Max(size.Width, _tabPanel->GetWidth());
        size.Height += _tabPanel->GetHeight();
    }
    SetSize(size);
}

void WindowShell::Arrange()
{
    // Title bar
    if (_titleBar != nullptr)
    {
        _titleBar->SetX(1);
        _titleBar->SetY(1);
        _titleBar->SetWidth(GetWidth() - 2);
    }

    // Close button
    if (_closeButton != nullptr)
    {
        _closeButton->SetWidth(11);
        _closeButton->SetHeight(12);
        if (_titleBar != nullptr)
        {
            _closeButton->SetHeight(_titleBar->GetHeight() - 2);
            _closeButton->SetWidth(_closeButton->GetHeight() - 1);
        }

        _closeButton->SetX(GetWidth() - _closeButton->GetWidth() - 2);
        _closeButton->SetY(2);

        _closeButton->ButtonFlags |= BUTTON_FLAGS::STYLE_LIGHT;
    }

    // Tab panel
    if (_tabPanel != nullptr)
    {
        _tabPanel->SetX(0);
        _tabPanel->SetY(17);
        if (_titleBar != nullptr)
        {
            _tabPanel->SetY(_titleBar->GetBottom() + 2);
        }

        _tabPanel->SetWidth(GetWidth());
        _tabPanel->SetHeight(GetHeight() - _tabPanel->GetY());
        _tabPanel->SetStyle(1);
        _tabPanel->Flags &= ~WIDGET_FLAGS::INHERIT_STYLE;
    }
}

TitleBar * WindowShell::GetTitleBar()
{
    return _titleBar;
}

Button * WindowShell::GetCloseButton()
{
    return _closeButton;
}

TabPanel * WindowShell::GetTabPanel()
{
    return _tabPanel;
}
