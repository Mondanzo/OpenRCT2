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

#include "../core/String.hpp"
#include "../object/ObjectManager.h"
#include "Sandbox.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../cheats.h"
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../world/park.h"
    #include "scenario.h"
}

extern "C"
{
    void sandbox_start()
    {
        audio_pause_sounds();
        audio_unpause_sounds();

        object_manager_unload_all_objects();
        object_list_load();

        game_init_all(72);

        gScreenAge = 0;
        gScreenFlags = SCREEN_FLAGS_PLAYING;
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;

        viewport_init_all();
        game_create_windows();
        rct_window *mainWindow = window_get_main();
        window_set_location(mainWindow, 69 * 32, 36 * 32, 112);
        load_palette();
        gfx_invalidate_screen();

        gCurrentRotation = 0;
        gCheatsSandboxMode = true;
        map_buy_land_rights(2 * 32, 2 * 32, 69 * 32, 69 * 32, 6, GAME_COMMAND_FLAG_APPLY | (OWNERSHIP_OWNED << 4));
        gCheatsSandboxMode = false;

        gScenarioObjectiveType = OBJECTIVE_HAVE_FUN;

        String::Set(gS6Info.name, sizeof(gS6Info.name), "Sandbox");
        String::Set(gS6Info.details, sizeof(gS6Info.details), "A box of sand.");

        park_set_name("Sandbox Park");

        scenario_begin();
    }
}
