#pragma once
#include <functional>
namespace __hash {
    long inthash(long long i) {
        return (i % 4294967296) ^ (i >> 32);
    }
};