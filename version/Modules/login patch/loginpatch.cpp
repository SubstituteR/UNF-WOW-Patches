#pragma once
#include "loginpatch.h"
#include <detours.h>
#include <fstream>
#include "json.hpp"
#include "../../wow.h"

using json = nlohmann::json;

bool loginPatch::isAuthenticating = false;
char loginPatch::username[16] = { 0 };
char loginPatch::password[16] = { 0 };

int* loginReady;
char* loginStep;

bool loginPatch::load()
{
    loginReady = (int*)0x76B474;
    loginStep = (char*)0x76AA38;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(WOW::Auth::Login.get(), callback);
    DetourAttach(WOW::Auth::LoginTick.get(), authCallback);
    DetourTransactionCommit();

    std::thread autoLogin([]() {
        char username[17] = "";
        char password[17] = "";
        try {
            nlohmann::json config;
            std::ifstream file("login.txt", std::ios::in);
            if (!file.is_open())
                return; //no point
            file >> config;
            file.close();
            strcpy_s(username, 16 * sizeof(char), config["username"].get<std::string>().c_str());
            strcpy_s(password, 16 * sizeof(char), config["password"].get<std::string>().c_str());
        }
        catch (const std::exception&)
        {
            printf("Invalid login.txt\n");
            return; //can't load
        }
        while (!*loginReady || !*WOW::Config::ToSAccepted || !*WOW::Config::EULAAccepted || *loginStep)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        WOW::Auth::Login(username, password);
    });
    autoLogin.detach();

    printf("Login Patch Applied!\n");
    return true;
}

bool loginPatch::unload()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(WOW::Auth::Login.get(), callback);
    DetourDetach(WOW::Auth::LoginTick.get(), authCallback);
    DetourTransactionCommit();
    printf("Login Patch Removed!\n");
    return true;
}

void loginPatch::callback(char* u, char* p)
{
    isAuthenticating = u[0] != 0 && p[0] != 0;
    if (isAuthenticating)
    {
        strcpy_s(username, 16 * sizeof(char), u);
        strcpy_s(password, 16 * sizeof(char), p);
        printf("User is authenticating to server...\n");
    }
    WOW::Auth::Login.original(u, p);
}

void loginPatch::authCallback()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (isAuthenticating)
    {
        switch (*WOW::Auth::LoginStatus)
        {
        case 4:
            SetConsoleTextAttribute(console, 2);
            printf("User account validated with server!\n");
            SetConsoleTextAttribute(console, 7);
            isAuthenticating = false;
            break;
        case 5:
            SetConsoleTextAttribute(console, 4);
            printf("User account did not validate with server!\n");
            SetConsoleTextAttribute(console, 7);
            isAuthenticating = false;
            break;
        default:
            break;
        }
    }
    WOW::Auth::LoginTick.original();
}
