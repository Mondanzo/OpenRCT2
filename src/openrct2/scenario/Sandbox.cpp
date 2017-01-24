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
#include "../object/ObjectRepository.h"
#include "Sandbox.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../cheats.h"
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../util/util.h"
    #include "../world/mapgen.h"
    #include "../world/park.h"
    #include "scenario.h"
}

static const char * DefaultObjects[] = {
    "TCF     ",
    "TRF     ",
    "TRF2    ",
    "TSP     ",
    "TMZP    ",
    "TAP     ",
    "TCRP    ",
    "TBP     ",
    "TCL     ",
    "TEL     ",
    "TWW     ",
    "TMP     ",
    "THL     ",
    "TH1     ",
    "TH2     ",
    "TPM     ",
    "TROPT1  ",
    "TBC     ",
    "TSC     ",
    "TCFS    ",
    "TNSS    ",
    "TRF3    ",
    "TRFS    ",
};

extern "C"
{
    void sandbox_start()
    {
        util_srand((uint32)time(0));

        audio_pause_sounds();
        audio_unpause_sounds();

        object_manager_unload_all_objects();
        object_list_load();

        IObjectRepository * objRepo = GetObjectRepository();
        IObjectManager * objManager = GetObjectManager();
        for (const char * objname : DefaultObjects)
        {
            auto ori = objRepo->FindObject(objname);
            if (ori != nullptr)
            {
                objManager->LoadObject(&ori->ObjectEntry);
            }
        }

        sint32 mapSize = 128;

        game_init_all(mapSize);

        mapgen_settings mapgenSettings = { 0 };
        mapgenSettings.mapSize = mapSize;
        mapgenSettings.height = 12;
        mapgenSettings.waterLevel = 12;
        mapgenSettings.floor = TERRAIN_GRASS;
        mapgenSettings.wall = TERRAIN_EDGE_ROCK;
        mapgenSettings.trees = 1;
        // mapgenSettings.simplex_low = 6;
        // mapgenSettings.simplex_high = 10;
        // mapgenSettings.simplex_base_freq = 60;
        // mapgenSettings.simplex_octaves = 4;
        mapgenSettings.simplex_low = util_rand() % 4;
        mapgenSettings.simplex_high = 12 + (util_rand() % 10);
        mapgenSettings.simplex_base_freq = 1.75f;
        mapgenSettings.simplex_octaves = 6;
        mapgen_generate(&mapgenSettings);

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
        map_buy_land_rights(2 * 32, 2 * 32, (mapSize - 3) * 32, (mapSize - 3) * 32, 6, GAME_COMMAND_FLAG_APPLY | (OWNERSHIP_OWNED << 4));
        gCheatsSandboxMode = false;

        gScenarioObjectiveType = OBJECTIVE_HAVE_FUN;

        String::Set(gS6Info.name, sizeof(gS6Info.name), "Sandbox");
        String::Set(gS6Info.details, sizeof(gS6Info.details), "A box of sand.");

        park_set_name("Sandbox Park");

        scenario_begin();
    }
}
