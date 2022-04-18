#pragma once
#include "vectors.hpp"
#include "nexusObject.hpp"
#include "win.hpp"
#include "Events.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

enum keyState {
    KEY_UP,
    KEY_DOWN
};
enum Key {
    LMB = 0x01,
    RMB = 0x02,
    CANCEL = 0x03,
    MMB = 0x04,
    X1MB = 0x05,
    X2MB = 0x06,
    BACKSPACE = 0x08,
    TAB = 0x09,
    CLEAR = 0x0C,
    ENTER = 0x0D,
    SHIFT = 0x10,
    CTRL = 0x11,
    ALT = 0x12,
    PAUSE = 0x13,
    CAPSLOCK = 0x14,
    IME_KANA = 0x15,
    IME_JUNJA = 0x17,
    IME_FINAL = 0x18,
    IME_HANJA = 0x19,
    IME_OFF = 0x1A,
    ESCAPE = 0x1B,
    IME_CONVERT = 0x1C,
    IME_NONCONVERT = 0x1D,
    IME_ACCEPT = 0x1E,
    IME_MODE_CHANGE_REQUEST = 0x1F,
    SPACE = 0x20,
    PAGEUP = 0x21,
    PAGEDOWN = 0x22,
    END = 0x23,
    HOME = 0x24,
    LEFT = 0x25,
    UP = 0x26,
    RIGHT = 0x27,
    DOWN = 0x28,
    SELECT = 0x29,
    PRINT = 0x2A,
    EXECUTE = 0x2B,
    PRINTSCREEN = 0x2C,
    INSERT_KEY = 0x2D,
    DELETE_KEY = 0x2E,
    KEY_0 = 0x30,
    KEY_1 = 0x31,
    KEY_2 = 0x32,
    KEY_3 = 0x33,
    KEY_4 = 0x34,
    KEY_5 = 0x35,
    KEY_6 = 0x36,
    KEY_7 = 0x37,
    KEY_8 = 0x38,
    KEY_9 = 0x39,
    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4A,
    K = 0x4B,
    L = 0x4C,
    M = 0x4D,
    N = 0x4E,
    O = 0x4F,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5A,
    LWIN = 0x5B,
    RWIN = 0x5C,
    APPS = 0x5D,
    SLEEP = 0x5F,
    NUMPAD0 = 0x60,
    NUMPAD1 = 0x61,
    NUMPAD2 = 0x62,
    NUMPAD3 = 0x63,
    NUMPAD4 = 0x64,
    NUMPAD5 = 0x65,
    NUMPAD6 = 0x66,
    NUMPAD7 = 0x67,
    NUMPAD8 = 0x68,
    NUMPAD9 = 0x69,
    MULTIPLY = 0x6A,
    ADD = 0x6B,
    SEPARATOR = 0x6C,
    SUBTRACT = 0x6D,
    DECIMAL = 0x6E,
    DIVIDE = 0x6F,
    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,
    F13 = 0x7C,
    F14 = 0x7D,
    F15 = 0x7E,
    F16 = 0x7F,
    F17 = 0x80,
    F18 = 0x81,
    F19 = 0x82,
    F20 = 0x83,
    F21 = 0x84,
    F22 = 0x85,
    F23 = 0x86,
    F24 = 0x87,
    NUMLOCK = 0x90,
    SCROLL = 0x91,
    LSHIFT = 0xA0,
    RSHIFT = 0xA1,
    LCONTROL = 0xA2,
    RCONTROL = 0xA3,
    LALT = 0xA4,
    RALT = 0xA5,
    BROWSER_BACK = 0xA6,
    BROWSER_FORWARD = 0xA7,
    BROWSER_REFRESH = 0xA8,
    BROWSER_STOP = 0xA9,
    BROWSER_SEARCH = 0xAA,
    BROWSER_FAVORITES = 0xAB,
    BROWSER_HOME = 0xAC,
    VOLUME_MUTE = 0xAD,
    VOLUME_DOWN = 0xAE,
    VOLUME_UP = 0xAF,
    MEDIA_NEXT_TRACK = 0xB0,
    MEDIA_PREV_TRACK = 0xB1,
    MEDIA_STOP = 0xB2,
    MEDIA_PLAY_PAUSE = 0xB3,
    LAUNCH_MAIL = 0xB4,
    LAUNCH_MEDIA_SELECT = 0xB5,
    LAUNCH_APP1 = 0xB6,
    LAUNCH_APP2 = 0xB7,
    OEM_1 = 0xBA,
    OEM_PLUS = 0xBB,
    OEM_COMMA = 0xBC,
    OEM_MINUS = 0xBD,
    OEM_PERIOD = 0xBE,
    OEM_2 = 0xBF,
    OEM_3 = 0xC0,
    OEM_4 = 0xDB,
    OEM_5 = 0xDC,
    OEM_6 = 0xDD,
    OEM_7 = 0xDE,
    OEM_8 = 0xDF,
    OEM_102 = 0xE2,
    IME_PROCESSKEY = 0xE5,
    ATTN = 0xF6,
    CRSEL = 0xF7,
    EXSEL = 0xF8,
    EREOF = 0xF9,
    PLAY = 0xFA,
    ZOOM = 0xFB,
    NONAME = 0xFC,
    PA1 = 0xFD,
    OEM_CLEAR = 0xFE,
    KEY_CODE_NONE = 0xFFFF,
};

namespace __windowprototype {
    class __nexusWindow 
    {
        WNDCLASSEX wc;
        friend class nexusWindow;
        HWND hWnd;
        LPCWSTR title;
        nexusWindow* n;
        __nexusWindow(HINSTANCE hInstance,LPCWSTR title, Vec2 resolution) {
            auto pClassName = L"hw3dbutts";
            this->title = title;
            FreeConsole();
            this->n = n;
            this->wc = {0};
            this->wc.cbSize = sizeof(this->wc);
            this->wc.style = CS_OWNDC;
            this->wc.lpfnWndProc = this->WndProc;
            this->wc.cbClsExtra = 0;
            this->wc.cbWndExtra = 0;
            this->wc.hInstance = hInstance;
            this->wc.hIcon = nullptr;
            this->wc.hCursor = nullptr;
            this->wc.hbrBackground = nullptr;
            this->wc.lpszMenuName = nullptr;
            this->wc.lpszClassName = pClassName;
            this->wc.hIconSm = nullptr;
            RegisterClassEx(&(this->wc));
            this->hWnd = CreateWindowEx(
                0,pClassName,
                title,
                WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                200,200,resolution.x,resolution.y,
                nullptr,nullptr,hInstance,nullptr);
            ShowWindow(this->hWnd,SW_SHOW);
        }
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    public:
        __nexusWindow() = delete;
        __nexusWindow(__nexusWindow&) = delete;
        __nexusWindow(__nexusWindow&&) = delete;
        __nexusWindow& operator=(__nexusWindow&) = delete;
    };
    Event<unsigned long long,unsigned long long> _KeyUpEvent;
    Event<unsigned long long,unsigned long long> _KeyDownEvent;
    Event<unsigned long long,unsigned long long> _KeyCharEvent;
    EventCaller<unsigned long long,unsigned long long> _KeyUpCall = _KeyUpEvent();
    EventCaller<unsigned long long,unsigned long long> _KeyDownCall = _KeyDownEvent();
    EventCaller<unsigned long long,unsigned long long> _KeyCharCall = _KeyCharEvent();
    class __nexusWindow;
    class nexusWindow;
    nexusWindow* inst = nullptr;
    __nexusWindow *_w = nullptr;
    class nexusWindow 
    {
    public:
        friend class __nexusWindow;
        void drawLine2D(Vec2, Vec2);
        void drawPolygon(Vec2, Vec2, Vec2);
        void drawLine3D(Vec3, Vec3);
        void drawPolygon(Vec3, Vec3, Vec3);
        HWND& __getHandle() {
            return _w->hWnd;
        }
        nexusWindow() = delete;
        nexusWindow(nexusWindow const&)     = delete;
        void operator=(nexusWindow const&)  = delete;
        static nexusWindow* newinst(HINSTANCE hInstance, string title, Vec2 res) {
            if (inst == nullptr) return new nexusWindow(hInstance,title,res,true);
            else return inst;
        }
        static nexusWindow* getinst() {
            return inst;
        }
        Event<unsigned long long,unsigned long long>& KeyUpEvent = _KeyUpEvent;
        Event<unsigned long long,unsigned long long>& KeyDownEvent = _KeyDownEvent;
        Event<unsigned long long,unsigned long long>& KeyCharEvent = _KeyCharEvent;
    protected:
        nexusWindow(HINSTANCE hInstance, string title, Vec2 res, bool);
        //static void KeyUpCall(unsigned long long a, unsigned long long b) {
        //    for (auto& i : nexusWindow::KeyUpEvent) {
        //        std::thread(nexusWindow::intercall,i,a,b).detach();
        //    }
        //}
        //static void KeyDownCall(unsigned long long a, unsigned long long b) {
        //    for (auto& i : nexusWindow::KeyDownEvent) {
        //        std::thread(nexusWindow::intercall,i,a,b).detach();
        //    }
        //}
        //static void KeyCharCall(unsigned long long a, unsigned long long b) {
        //    for (auto& i : nexusWindow::KeyCharEvent) {
        //        std::thread(nexusWindow::intercall,i,a,b).detach();
        //    }
        //}
        //static void intercall(void (*ev)(unsigned long long,unsigned long long), unsigned long long a, unsigned long long b) {
        //    std::unique_lock<std::mutex> lck(nexusWindow::mtx);
        //    cv.wait(lck);
        //    ev(a,b);
        //}
        //static std::mutex mtx;
        //static std::condition_variable cv;
    };
    nexusWindow::nexusWindow(HINSTANCE hInstance, string title, Vec2 res, bool) {
        std::wstring stemp = std::wstring(title.begin(), title.end());
        LPCWSTR sw = stemp.c_str();
        _w = new __nexusWindow(hInstance, sw, res);
    }
    LRESULT CALLBACK __nexusWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_CLOSE:
                PostQuitMessage(0);
                break;
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_KEYDOWN:
                OutputDebugString((L"Key down: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyDownCall(wParam,lParam);
                return 0;
            case WM_LBUTTONUP:
                OutputDebugString((L"Key up LB: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyUpCall(Key::LMB,lParam);
                return 0;
            case WM_MBUTTONUP:
                OutputDebugString((L"Key up MB: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyUpCall(Key::MMB,lParam);
                return 0;
            case WM_RBUTTONUP:
                OutputDebugString((L"Key up RB: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyUpCall(Key::RMB,lParam);
                return 0;
            case WM_KEYUP:
                OutputDebugString((L"Key up: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyUpCall(wParam,lParam);
                return 0;
            case WM_CHAR:
                OutputDebugString((L"Char: " + std::to_wstring(wParam) + L"\n").c_str());
                _KeyCharCall(wParam,lParam);
                return 0;
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        };
}
using __windowprototype::nexusWindow;