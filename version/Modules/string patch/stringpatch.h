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

inline hookable<const char* (const char*, int, int)> loadString((void*)0x419D40);

class stringPatch : public IPatch
{
public:
    static std::map<const char*, const char*, mapComp> stringOverrides;
    bool load();
    bool unload();
    static const char* callback(const char*, int, int);
};