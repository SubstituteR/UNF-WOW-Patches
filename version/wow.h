#pragma once
#include "hookableFunction.h"
#include "extern_ptr.h"
namespace WOW
{
	namespace Config
	{
		inline extern_ptr<int> ToSAccepted(TEXT("wow.exe"), 0x76AF54);
		inline extern_ptr<int> EULAAccepted(TEXT("wow.exe"), 0x76AF5C);
	}
	namespace Auth
	{
		inline int* const LoginStatus = (int*)0x6C3DA4;
		inline hookable<void(char*,char*)> Login(TEXT("wow.exe"), 0xD8A30);
		inline hookable<void()> LoginTick(TEXT("wow.exe"), 0xDAB40);
	}
}