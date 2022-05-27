#define RUN_NEXUSENGINE_AS_MAIN
#ifdef RUN_NEXUSENGINE_AS_MAIN
#include "nexusENGINE.hpp"
int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, 
                     int nCmdShow)
{   
    //init();
    #ifndef NDEBUG
    nexusWindow *w = nexusWindow::newinst(hInstance,"nexusEngine - Debug Mode",{640,480});
    #else
    nexusWindow *w = nexusWindow::newinst(hInstance,"nexusEngine",{640,480});
    #endif
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
    return 0;
}
#endif