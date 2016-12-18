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

#include <string>
#include "../Widget.h"

namespace OpenRCT2::Ui
{
    /**
     * The title bar widget displaying the title of the window, seen in at
     * the top of most widgets. Moves the position of the window when dragged.
     */
    class TitleBar : public Widget
    {
    private:
        std::string _text;
        bool        _movingWindow = false;
        xy32        _lastCursorPosition = { 0 };

    public:
        std::string GetText();
        void SetText(const std::string &value);

        void Measure() override;

        void Draw(IDrawingContext * dc) override;

        void MouseDown(const MouseEventArgs * e) override;
        void MouseMove(const MouseEventArgs * e) override;
        void MouseUp(const MouseEventArgs * e) override;
    };
}
