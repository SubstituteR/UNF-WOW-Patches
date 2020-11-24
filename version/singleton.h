#pragma once
template<typename T>
class Singleton {
private:
    Singleton();
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static auto& get() {
        static T _;
        return _;
    }
};