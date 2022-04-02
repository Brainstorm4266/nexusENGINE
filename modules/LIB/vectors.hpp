#pragma once
#include <initializer_list>
struct Vec2 {
    long double x,y;
    Vec2(long double x, long double y) : x(x), y(y) {}
    Vec2(std::initializer_list<long double> l) : x(*l.begin()), y(*(l.begin()+1)) {}
};
struct Vec3 {
    long double x,y,z;
    Vec3(long double x, long double y, long double z) : x(x), y(y), z(z) {}
    Vec3(std::initializer_list<long double> l) : x(*l.begin()), y(*(l.begin()+1)), z(*(l.begin()+2)) {}
};