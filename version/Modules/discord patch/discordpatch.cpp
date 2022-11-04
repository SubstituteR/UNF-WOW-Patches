/*

#pragma once
#include "discordpatch.h"
#include <detours.h>
#include "../picosha2.h"
#include "../wowFunctions.h"
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

bool discordPatch::running = false;
DiscordState discordPatch::state;
gameState discordPatch::worldState{};

std::string computePassword(discord::UserId account, discord::OAuth2Token token)
{
	std::vector<unsigned char> hash(picosha2::k_digest_size);
	std::stringstream data;
	data << account << ":" << token.GetAccessToken();
	std::string str(data.str());
	picosha2::hash256(str.begin(), str.end(), hash.begin(), hash.end());
	return picosha2::bytes_to_hex_string(hash.begin(), hash.end()).substr(0, 16);
}

std::string computeUsername(discord::UserId account)
{
	std::stringstream data;
	data << std:: hex << account;
	return data.str();
}


void updateStatus(DiscordState& state, discord::Activity& activity)
{
	std::condition_variable cv;
	discord::Result res;
	auto callback = [&](discord::Result result) {
		res = result;
		state.currentStatus = activity;
		cv.notify_one();
	};
	state.core->ActivityManager().UpdateActivity(activity, callback);
	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);
	cv.wait(lck);
	std::cout << ((res == discord::Result::Ok) ? "Succeeded" : "Failed") << " updating activity!\n";
}

bool discordPatch::load()
{


	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	//DetourAttach(&WOW::Game::ZoneChanged, callback);
	//DetourAttach(&WOW::Game::GroupMemberJoined, groupPlayerJoined);
	//DetourAttach(&WOW::Game::GroupResetting, groupResetting);
	//DetourAttach(&WOW::Game::GroupLeaderChanged, groupLeaderChanged);
	//DetourAttach(&WOW::Game::UpdatePlayerPointer, characterPointerUpdated);
	DetourTransactionCommit();


	discord::Core* core = nullptr;
	auto result = discord::Core::Create(775104260517789696, DiscordCreateFlags_Default, &core);
	state.core.reset(core);
	if (!state.core)
	{
	std::cout << "Failed to init Discord, " << static_cast<int>(result) << "." << std::endl;
	return false;
	}

	core->UserManager().OnCurrentUserUpdate.Connect([&]() {
		state.core->UserManager().GetCurrentUser(&state.currentUser);
		});

		running = true;
		std::thread([]() {
			while (running)
			{
				auto partyStatus = processVariable<int>(NULL, 0x774484);
				auto partyCount = processVariable<int>(NULL, 0x7744A4);
				auto playerName = processVariable<char>(NULL, 0x879D18);


				if (state.core)
					state.core->RunCallbacks();
				std::this_thread::sleep_for(std::chrono::milliseconds(16));

			}
			}).detach();

			discord::Activity activity = state.currentStatus;
			activity.SetDetails("");
			activity.SetState("Gaming");
			activity.GetAssets().SetLargeImage("logo");
			activity.SetType(discord::ActivityType::Playing);
			updateStatus(state, activity);
			return true;
}


//wow.exe+12D6E0 removed member <-------- wipes the party / resets the party
//wow.exe+2D8870 reset entire party ?

//wow.exe+12D454 update party layout (?)
//wow.exe+125834 party disbanded ?
bool discordPatch::unload()
{
	running = false;
	return true;
}

void* discordPatch::groupPlayerJoined(UINT64 charId, int a2, const void* a3, char a4, int a5, int a6)
{
	worldState.partyMembers.insert(charId);
	return WOW::Game::GroupMemberJoined.original(charId, a2, a3, a4, a5, a6);;
}

int discordPatch::groupResetting(int i)
{
	worldState.partyMembers.clear();
	return WOW::Game::GroupResetting.original(i);
}

void discordPatch::groupLeaderChanged(UINT64 charId)
{
	worldState.partyLeader = charId;
	auto activity = state.currentStatus;
	if (charId == 0)  // When this is zero, the party is disbanded!
	{
		activity.GetParty().SetId("");
		activity.GetParty().GetSize().SetCurrentSize(0);
		activity.GetParty().GetSize().SetMaxSize(0);
	}
	else {
		activity.GetParty().SetId(""); //* todo -> do party id when we implement invite/join button on Discord.
		activity.GetParty().GetSize().SetCurrentSize(worldState.partyMembers.size() + 1);
		activity.GetParty().GetSize().SetMaxSize(5);
	}


	updateStatus(state, activity);
	WOW::Game::GroupLeaderChanged.original(charId);
}

int discordPatch::characterPointerUpdated(WOW::Game::GameManager* data, UINT64 a2)
{
	if (worldState.globalState != data)
	{
		worldState.globalState = data;
		discord::Activity activity = state.currentStatus;
		if (data)
		{
			auto player = data->worldManager->playerManager->player;
			std::stringstream charText{};
			charText << WOW::Game::PlayerName() << " - Level "
				<< player->Level
				<< " " << WOW::Game::Races[player->Race]
				<< " " << WOW::Game::Classes[player->Class];
			std::string key = WOW::Game::Classes[player->Class];
			transform(key.begin(), key.end(), key.begin(), ::tolower);
			std::cout << "setting small icon key to " << key << std::endl;
			activity.GetAssets().SetSmallImage(key.c_str());
			activity.GetAssets().SetSmallText(charText.str().c_str());
			if (WOW::Game::isAlliance(player->Race))
				activity.GetAssets().SetLargeImage("logo_alliance");
			else
				activity.GetAssets().SetLargeImage("logo_horde");
		}
		else {
			printf("no data....logged out\n"); //unused?
			activity.GetAssets().SetSmallImage(nullptr);
			activity.GetAssets().SetSmallText(nullptr);
		}
		updateStatus(state, activity);
	}
	return WOW::Game::UpdatePlayerPointer.original(data, a2);
}

const int discordPatch::callback(int inZoneId, int inSubZoneId, int inCityZoneId, const char* inZoneName, const char* inSubZoneName, const char* inCityZoneName, int isCity)
{
	std::stringstream newZone{};
	if (inZoneId)
	{
		newZone << "Exploring ";
		if (inSubZoneName)
			newZone << inZoneName << " - " << inSubZoneName;
		else
			newZone << inZoneName;
	}
	else {
		newZone << "Character Select";
	}

	auto activity = state.currentStatus;
	activity.SetDetails(newZone.str().c_str());
	activity.SetType(discord::ActivityType::Playing);

	updateStatus(state, activity);
	return WOW::Game::ZoneChanged.original(inZoneId, inSubZoneId, inCityZoneId, inZoneName, inSubZoneName, inCityZoneName, isCity);;
}



*/