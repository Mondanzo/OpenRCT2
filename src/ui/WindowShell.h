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

#include <vector>
#include "widgets/Panel.h"

namespace OpenRCT2::Ui
{
    class Button;
    class TabPanel;
    class TitleBar;

    /**
     * The shell for the window containing common widgets such as the title bar, close button
     * and tab control.
     */
    class WindowShell : public Panel
    {
    private:
        TitleBar *  _titleBar = nullptr;
        Button *    _closeButton = nullptr;
        TabPanel *  _tabPanel = nullptr;

    public:
        WindowShell(Window * window);
        ~WindowShell();

        void Initialise();

        void Measure() override;
        void Arrange() override;

        TitleBar * GetTitleBar();
        Button * GetCloseButton();
        TabPanel * GetTabPanel();
    };
}
