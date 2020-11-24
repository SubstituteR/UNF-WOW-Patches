#pragma once
#include <map>
#include <json.hpp>
#include <patches.h>
#include "../hookableFunction.h"

class mapComp
{
public:
    bool operator()(const char* a, const char* b) const
    {
        return strcmp(a, b) < 0;
    }
};

HOOKABLE(loadString_t, const char*, const char*, int, int)
class stringPatch : public IPatch
{
public:
    static loadString_t loadString;
    static std::map<const char*, const char*, mapComp> stringOverrides;
    bool load();
    bool unload();
    static const char* callback(const char*, int, int);
};