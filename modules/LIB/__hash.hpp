#pragma once
#include <functional>
namespace __hash {
    #define hash_f std::hash
    unsigned long inthash(unsigned long long i) {
        return (i % 4294967296) ^ (i >> 32);
    }
};