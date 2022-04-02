#pragma once
#include "vectors.hpp"
#include <Windows.h>

class __nexusWindow 
{
private:
    friend class nexusWindow;
    WNDCLASSEX* wc;
    __nexusWindow(HINSTANCE hInstance) {
        this->wc = (WNDCLASSEX*)malloc(sizeof(WNDCLASSEX));
        this->wc->cbSize = sizeof(WNDCLASSEX);
        this->wc->style = CS_OWNDC;
        this->wc->lpfnWndProc = DefWindowProc;
        this->wc->cbClsExtra = 0;
        this->wc->cbWndExtra = 0;

    }
};
class nexusWindow 
{
public:
    nexusWindow(HINSTANCE hInstance) {
        this->_w = new __nexusWindow(hInstance);
    }
    void drawLine2D(Vec2, Vec2);
    void drawPolygon(Vec2, Vec2, Vec2);
    void drawLine3D(Vec3, Vec3);
    void drawPolygon(Vec3, Vec3, Vec3);
protected:
    __nexusWindow *_w;
};