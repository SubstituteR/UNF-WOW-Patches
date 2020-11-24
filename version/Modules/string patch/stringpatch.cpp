#pragma once
#include "stringpatch.h"
#include <detours.h>
#include "../wowFunctions.h"

loadString_t stringPatch::loadString;
bool stringPatch::load()
{
    loadString = loadString_t(NULL, 0x419D40);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&loadString, callback);
    DetourTransactionCommit();
    printf("This is where our patch is applied!\n");
    return true;
}

bool stringPatch::unload() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&loadString, callback);
    DetourTransactionCommit();
    printf("This is where our patch is removed!\n");
    return true;
}

std::map<const char*, const char*, mapComp> stringPatch::stringOverrides = 
    {
        {"ACCOUNT_NAME", "Game Servers Account Name"},
        {"ENTER_EMAIL", "Enter Username"},
        {"BLIZZ_DISCLAIMER", "Welcome to the University of North Florida WOW Private Server!"},
        {"SERVER_ALERT_URL", "http://127.0.0.1/alert"},
        {"LOGIN_UNKNOWN_ACCOUNT_68", "<html><body><p align=\"CENTER\">This Discord account isn't registered, visit <a href=\"https://register.unfesports.com\">https://register.unfesports.com</a> for more information.</p></body></html>"},
    };

const char* stringPatch::callback(const char* key, int a2, int a3)
{
    const char* result = 0;
    if (stringOverrides.find((const char*)key) == stringOverrides.end())
        result = loadString.original(key, a2, a3);
    else
        result = stringOverrides[key];
    return result;
}
