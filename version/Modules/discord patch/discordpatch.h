#pragma once
#include <map>
#include <set>
#include <json.hpp>
#include <patches.h>
#include "discord/discord.h"
#include "../hookableFunction.h"
#include "../../wowFunctions.h"

HOOKABLE(loadString_t, const char*, const char*, int, int)

struct DiscordState {
    discord::User currentUser;
    discord::Activity currentStatus{};
    std::unique_ptr<discord::Core> core;
};

enum movementModes
{
    forwards = 1,
    backwards = 2,
    strafeleft = 4,
    straferight = 8,
    turnleft = 16,
    turnright = 32,
    jump = 4096,
    falling = 12288,
    swimming = 2097152,
    fly = 16777216
};

struct gameState {
    std::set<UINT64> partyMembers;
    UINT64 partyLeader;
    WOW::Game::GameManager* globalState;
};

class discordPatch   : public IPatch
{
    static bool running;
    static DiscordState state;
public:
    static gameState worldState;
    bool load();
    bool unload();
    static const int callback(int, int, int, const char*, const char*, const char*, int);

    static void* groupPlayerJoined(UINT64, int, const void*, char, int, int);
    static int groupResetting(int);
    static void groupLeaderChanged(UINT64);
    static int characterPointerUpdated(WOW::Game::GameManager* data, UINT64 a2);

    static DWORD SignalEvent(int eventId, char* args, ...);
};