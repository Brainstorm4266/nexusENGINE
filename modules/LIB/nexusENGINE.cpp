#define RUN_NEXUSENGINE_AS_MAIN
#ifdef RUN_NEXUSENGINE_AS_MAIN
#include "nexusENGINE.hpp"
int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, 
                     int nCmdShow)
{   
    #ifndef NDEBUG
    nexusWindow *w = nexusWindow::newinst("nexusEngine - Debug Mode",{640,480});
    #else
    nexusWindow *w = nexusWindow::newinst("nexusEngine",{640,480});
    #endif
    w->KeyDownEvent.Connect([](unsigned long long a, unsigned long long b) {
        debug_print("Event call key down:",a,b);
        debug_print("Keys down:",nexusWindow::getKeysDown());
    });
    w->KeyUpEvent.Connect([](unsigned long long a, unsigned long long b) {
        debug_print("Event call key up:",a);
        debug_print("Keys down:",nexusWindow::getKeysDown());
    });
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
}
#endif