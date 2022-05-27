#pragma once
#include "win.hpp"
#include <d3d11.h>

class Graphics
{
public:
    Graphics (HWND hWnd);
private:
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwap = nullptr;
};