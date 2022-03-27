#pragma once
#include<vector>
template <typename T>
bool isInVector(std::vector<T> l, T v) {
    for (T i : l) {
        if (i == v) return true;
    }
    return false;
}