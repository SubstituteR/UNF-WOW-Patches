#pragma once
#include "hookableFunction.h"

namespace WOW
{
	namespace Game {
		struct GameManager;
	}
}

HOOKABLE(login_t, void, char*, char*)
HOOKABLE_VA(msgbox_t, int, int, const char*)
HOOKABLE(loginCallbacks_t, void)
HOOKABLE(zonechange_t, int, int, int, int, const char*, const char*, const char*, int)
HOOKABLE(group_CharacterJoin_t, void*, UINT64, int, const void*, char, int, int)
HOOKABLE(group_Reset_t, int, int)
HOOKABLE(group_LeaderChanged_t, void, UINT64)
HOOKABLE(set_playerPointer_t, int, WOW::Game::GameManager*, UINT64)

namespace WOW
{
	namespace Config
	{
		extern processVariable<int> ToSAccepted;
		extern processVariable<int> EULAAccepted;
	}
	namespace Auth
	{
		extern login_t Login;
		extern processVariable<int> LoginStatus;
		extern loginCallbacks_t LoginCallbacks;
	}
	namespace GUI
	{
		extern msgbox_t MsgBox;
	}
	namespace Game
	{
		static const char* Classes[] = {
			"Player", //Default
			"Warrior",
			"Paladin",
			"Hunter",
			"Rogue",
			"Priest",
			"Death Knight",
			"Shaman",
			"Mage",
			"Warlock"
		};

		static const char* Races[] = {
			"",
			"Human",
			"Orc",
			"Dwarf",
			"Night Elf",
			"Undead",
			"Tauren",
			"Gnome",
			"Troll",
			"", /* unused */
			"Blood Elf",
			"Draenai"
		};

		static int isAlliance(int raceId)
		{
			switch (raceId)
			{
			case 1: //human
				return 1;
			case 2: //orc
				return 0;
			case 3: //dwarf
			case 4: //nightelf
				return 1;
			case 5: //undead
			case 6: //tauren
				return 0;
			case 7: //gnome
				return 1;
			case 8: //troll
			case 10: //bloodelf
				return 0;
			case 11: //draenei
				return 1;
			default:
				break;
			}
		}

		struct GroupInfo
		{
			UINT64 members[4];
			UINT64 leader;
		};
		struct PlayerData
		{
			char pad[0x5C];
			char Race;
			char Class;
			char isFemale;
			char pad2[0x76];
			int Level;
		};
		struct PlayerManager
		{
			char pad[0x8];
			PlayerData* player;
		};
		struct WorldManager
		{
			char pad[0x24];
			PlayerManager* playerManager;
		};
		struct GameManager
		{
			char pad[0x34];
			WorldManager* worldManager;
		};
		extern zonechange_t ZoneChanged;

		extern processVariable<int> PlayerGroupMemberCount;
		extern processVariable<GroupInfo> PlayerGroup;
		extern processVariable<char*> PlayerName;

		extern group_CharacterJoin_t GroupMemberJoined;
		extern group_LeaderChanged_t GroupLeaderChanged;
		extern group_Reset_t GroupResetting; //This is called when the group is being re-created due to an update.

		extern set_playerPointer_t UpdatePlayerPointer; //Runs every tick.
	}
}

