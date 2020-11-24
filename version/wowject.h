#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <string patch/stringpatch.h>
#include <login patch/loginpatch.h>
#include "wowFunctions.h"
#include <discord patch\discordpatch.h>

login_t WOW::Auth::Login = login_t(nullptr);
msgbox_t WOW::GUI::MsgBox = msgbox_t(nullptr);
processVariable<int> WOW::Config::ToSAccepted = processVariable<int>(nullptr);
processVariable<int> WOW::Config::EULAAccepted = processVariable<int>(nullptr);
processVariable<int> WOW::Auth::LoginStatus = processVariable<int>(nullptr);

loginCallbacks_t WOW::Auth::LoginCallbacks = loginCallbacks_t(nullptr);

zonechange_t WOW::Game::ZoneChanged = zonechange_t(nullptr);

group_CharacterJoin_t WOW::Game::GroupMemberJoined = group_CharacterJoin_t(nullptr);
group_Reset_t WOW::Game::GroupResetting = group_Reset_t(nullptr);
group_LeaderChanged_t WOW::Game::GroupLeaderChanged = group_LeaderChanged_t(nullptr);

set_playerPointer_t WOW::Game::UpdatePlayerPointer = set_playerPointer_t(nullptr);

processVariable<int> WOW::Game::PlayerGroupMemberCount = processVariable<int>(nullptr);
processVariable<WOW::Game::GroupInfo> WOW::Game::PlayerGroup = processVariable<WOW::Game::GroupInfo>(nullptr);
processVariable<char*> WOW::Game::PlayerName = processVariable<char*>(nullptr);
void initFunctions()
{
	WOW::Auth::Login = login_t(NULL, 0xD8A30);
	WOW::Auth::LoginCallbacks = loginCallbacks_t(NULL, 0xDAB40);
	WOW::Auth::LoginStatus = processVariable<int>(NULL, 0x6C3DA4);

	WOW::GUI::MsgBox = msgbox_t(NULL, 0x41B530); /* this is the LUA event signaller !*/

	WOW::Config::ToSAccepted = processVariable<int>(NULL, 0x76AF54);
	WOW::Config::EULAAccepted = processVariable<int>(NULL, 0x76AF5C);

	WOW::Game::ZoneChanged = zonechange_t(NULL, 0x1204C0);

	WOW::Game::GroupMemberJoined = group_CharacterJoin_t(NULL, 0x12D410);
	WOW::Game::GroupResetting = group_Reset_t(NULL, 0x12D6E0);
	WOW::Game::GroupLeaderChanged = group_LeaderChanged_t(NULL, 0x12BC80);

	WOW::Game::UpdatePlayerPointer = set_playerPointer_t(NULL, 0x380500);

	WOW::Game::PlayerGroupMemberCount = processVariable<int>(NULL, 0x07744A4);
	WOW::Game::PlayerGroup = processVariable<WOW::Game::GroupInfo>(NULL, 0x7D1948);
	WOW::Game::PlayerName = processVariable<char*>(NULL, 0x879D18);
}

void Init()
{
	initFunctions();
	Patches::Add<stringPatch>();
	Patches::Add<loginPatch>();
	Patches::Add<discordPatch>();
}