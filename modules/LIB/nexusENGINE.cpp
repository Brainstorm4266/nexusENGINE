#include <Windows.h>

#include "nexusENGINE.hpp"
#define RUN_NEXUSENGINE_AS_MAIN
#ifdef RUN_NEXUSENGINE_AS_MAIN
int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, 
                     int nCmdShow)
{   
    init();
    nexusWindow *w = new nexusWindow(hInstance);
    return 0;
}
#endif