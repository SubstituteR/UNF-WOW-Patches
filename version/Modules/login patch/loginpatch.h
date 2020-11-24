#pragma once
#include <json.hpp>
#include <patches.h>
#include "../hookableFunction.h"

class loginPatch : public IPatch
{
public:
    bool load();
    bool unload();
    static bool isAuthenticating;
    static char username[];
    static char password[];
    static void callback(char* username, char* password);
    static void authCallback();
};