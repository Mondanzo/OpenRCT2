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

#include "../common.h"

interface INetworkClient;
interface INetworkContext;
interface INetworkGroupManager;
interface INetworkPlayerList;
interface INetworkServer;
interface INetworkUserManager;

enum NETWORK_MODE
{
    NETWORK_MODE_NONE,
    NETWORK_MODE_CLIENT,
    NETWORK_MODE_SERVER,
};

namespace Network2
{
    INetworkClient *        GetClient();
    INetworkContext *       GetContext();
    INetworkGroupManager *  GetGroupManager();
    INetworkPlayerList *    GetPlayerList();
    INetworkServer *        GetServer();
    INetworkUserManager *   GetUserManager();

    NETWORK_MODE GetMode();

    bool Initialise();
    void Dispose();
    void Update();

    INetworkServer * BeginServer(uint16 port);
    INetworkClient * BeginClient(const char * host, uint16 port);
}

// This define specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION  "11"
#define NETWORK_STREAM_ID       (OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION)
