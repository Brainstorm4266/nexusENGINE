#pragma once
#include "vectors.hpp"
class __nexusWindow;
class nexusWindow {
public:
    void drawLine2D(Vec2, Vec2);
    void drawLine3D(Vec3, Vec3);
    void drawPolygon(Vec3, Vec3, Vec3);
protected:
    __nexusWindow *_w;
};