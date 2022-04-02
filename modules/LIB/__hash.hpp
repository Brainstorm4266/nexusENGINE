#pragma once
#include <functional>
namespace __hash {
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    unsigned long long inthash(unsigned long long a, unsigned long long b) {
        std::size_t s;
        hash_combine(s, a);
        hash_combine(s, b);
        return s;
    };
};
