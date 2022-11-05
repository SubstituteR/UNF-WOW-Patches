#pragma once
#include "version.h"
#include "hookableFunction.h"
#include "wowFunctions.h"
#include "wowject.h"
#include <string>
#include <thread>
#include <iostream>
#include "Modules/patches.h"
#include "Modules/discord patch/discordpatch.h"

void showConsole()
{
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    std::thread consoleThread([]()
        {
            std::string input;
            for (;;)
            {
                std::getline(std::cin, input);
                if (input.compare("patches") == 0)
                {
                    printf("::: Loaded Patches :::\n");
                    Patches::Dump();
                }
                if (input.compare("party") == 0)
                {
                    /*
                    if (discordPatch::worldState.partyMembers.size() == 0)
                        printf("Player is not in a party.\n");
                    else {
                        printf("party has leader: %i\n", discordPatch::worldState.partyLeader);
                        for (int player : discordPatch::worldState.partyMembers)
                        {
                            printf("party has member: %i\n", player);
                        }
                    }
                    */
                }
            }
        });
    consoleThread.detach();
}


BOOL WINAPI InitHook()
{

    if (!CreateThread(0, 0, [](LPVOID)->DWORD
        {

            showConsole();
            Init();
            //int below
            //load_motd_orig = (void*)((int)GetModuleHandleW(NULL) + 0x1B60);
            //load_agreements_orig = (void*)((int)GetModuleHandleW(NULL) + 0xDA360);
            return TRUE;
        }, 0, 0, 0))
        return FALSE;
        return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        setupWrappers();
        InitHook();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

