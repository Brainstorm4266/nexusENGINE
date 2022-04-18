#define RUN_NEXUSENGINE_AS_MAIN
#ifdef RUN_NEXUSENGINE_AS_MAIN
#include "nexusENGINE.hpp"
void test_event_k_down(unsigned long long a, unsigned long long b) {
    OutputDebugString((L"Event call key down: " + std::to_wstring(a) + L" " + std::to_wstring(b) + L"\n").c_str());
}
int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, 
                     int nCmdShow)
{   
    #ifdef oldv
    #ifndef NDEBUG
    nexusWindow *w = nexusWindow::newinst(hInstance,"nexusEngine - Debug Mode",{640,480});
    #else
    nexusWindow *w = nexusWindow::newinst(hInstance,"nexusEngine",{640,480});
    #endif
    w->KeyDownEvent.Connect(test_event_k_down);
    MSG msg;
    BOOL gResult;
    while (gResult = GetMessage(&msg,nullptr,0,0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (gResult == -1) {
        return -1;
    } else {
        return msg.wParam;
    }
    #else
        __windowprototype2::nexusWindow::nexusWindow(640,480,L"OwO");
        __windowprototype2::nexusWindow::nexusWindow(480,640,L"UwU");
    #endif
    return 0;
}
#endif