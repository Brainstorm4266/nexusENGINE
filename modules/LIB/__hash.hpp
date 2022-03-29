#pragma once
#include <functional>
namespace __hash {
    #define hash_f std::hash
    struct l2 {
        unsigned long long a,b;
        l2(unsigned long long a, unsigned long long b) : a(a), b(b) {}
    };
    std::hash<l2> ls;
    unsigned long long inthash(unsigned long long a, unsigned long long b) {
        return ls._Do_hash(l2(a,b));
    }
};
