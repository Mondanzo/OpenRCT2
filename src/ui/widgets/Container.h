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

#include "../Widget.h"

namespace OpenRCT2 { namespace Ui
{
    /**
     * Basic container that is filled with it's child.
     */
    class Container : public Widget
    {
    private:
        Widget * _child;

    public:
        Widget * GetChild();
        void SetChild(Widget * widget);
        sint32 GetChildrenCount() override;
        Widget * GetChild(sint32 index) override;

        void Arrange() override;
    };
} }
