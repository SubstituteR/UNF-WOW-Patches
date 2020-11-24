#pragma once
#include <discord/discord.h>

struct DiscordState {
    discord::User currentUser;
    std::unique_ptr<discord::Core> core;
};