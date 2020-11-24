#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef UNICODE
#define WIN_STR LPCWSTR
#else
#define WIN_STR  LPCSTR
#endif
#include <type_traits>
struct VA_ARGS {};
struct NO_VA {};
template<typename RT, typename VA, typename ...A>
class hookableFunction
{
    LPVOID immutable_ptr;
    LPVOID address_ptr;
public:
    hookableFunction(LPVOID address)
    {
        this->address_ptr = address;
        this->immutable_ptr = address;
    }
    hookableFunction(WIN_STR module, int offset) : hookableFunction((LPVOID)((int)GetModuleHandle(module) + offset)) {} /* module + offset */
    hookableFunction() : hookableFunction(nullptr) {}; /* No Arg Constructor */

    const LPVOID immutable() {
        return immutable_ptr; /* Read-Only copy of the original pointer value */
    }
    const LPVOID address() {
        return address_ptr; /* Read-Only copy of the address */
    }

    LPVOID* operator&() {
        return (LPVOID*)&address_ptr;
    }
    
    template<class = typename std::enable_if<std::is_same<VA, NO_VA>::value>::type>
    RT operator()(A... args) {
        if (!immutable_ptr) throw "Attempted to call null pointer.";
        return ((RT(*)(A...)) immutable_ptr)(args...); /* This calls the original address at all times. IF you hook the func, this will include your hook. */
    }

    template<typename ...B, class = typename std::enable_if<std::is_same<VA, VA_ARGS>::value>::type>
    RT operator()(A... args, B... va) {
        if (!immutable_ptr) throw "Attempted to call null pointer.";
        return ((RT(*)(A..., B...)) immutable_ptr)(args..., va...); /* This calls the original address at all times. IF you hook the func, this will include your hook. */
    }

    template<class = typename std::enable_if<std::is_same<VA, NO_VA>::value>::type>
    RT original(A... args)
    {
        if (!address_ptr) throw "Attempted to call null pointer.";
        return ((RT(*)(A...)) address_ptr)(args...); /* This calls the updated pointer IF you hook the func. This is what you want to call from your hook. */
    }

    template<typename ...B, class = typename std::enable_if<std::is_same<VA, VA_ARGS>::value>::type>
    RT original(A... args, B... va)
    {
        if (!address_ptr) throw "Attempted to call null pointer.";
        return ((RT(*)(A..., B...)) address_ptr)(args..., va...); /* This calls the updated pointer IF you hook the func. This is what you want to call from your hook. */
    }
};

template<typename T>
class processVariable
{
    void* immutable_ptr;
public:
    processVariable(void* address)
    {
        this->immutable_ptr = address;
    }
    processVariable(WIN_STR module, int offset) : processVariable((void*)((int)GetModuleHandle(module) + offset)) {} /* module + offset */
    processVariable() : processVariable(nullptr) { printf("no arg constructed\n"); }; /* No Arg Constructor */

    T operator*() {
        if (!immutable_ptr)
            throw "Attempting to read null pointer.";
        return *(T*)immutable_ptr;
    }

    T* operator&() {
        return (T*) immutable_ptr;
    }

    T operator()() {
        return (T)immutable_ptr;
    }
};

#define HOOKABLE(name, rt, ...) \
using name = hookableFunction<rt, NO_VA, __VA_ARGS__>;
#define HOOKABLE_VA(name, rt, ...) \
using name = hookableFunction<rt, VA_ARGS, __VA_ARGS__>;