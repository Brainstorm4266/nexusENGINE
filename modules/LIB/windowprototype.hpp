#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "vectors.hpp"
#include "nexusObject.hpp"
#include "win.hpp"
#include "Events.hpp"
#include "../DEBUG/debug.hpp"
#include "nxeExc.hpp"
#include "isInVector.hpp"


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
        HINSTANCE hInstanceV;
        nexusWindow* n;
        __nexusWindow(LPCWSTR title, Vec2 resolution)
            : title(title), hInstanceV(GetModuleHandle(nullptr)), n(nullptr)
        {
            auto pClassName = L"hw3dbutts"; // ok what is this supposed to mean?!
            FreeConsole();
            this->wc = {0};
            this->wc.cbSize = sizeof(this->wc);
            this->wc.style = CS_OWNDC;
            this->wc.lpfnWndProc = this->WndProc;
            this->wc.cbClsExtra = 0;
            this->wc.cbWndExtra = 0;
            this->wc.hInstance = hInstanceV;
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
                nullptr,nullptr,hInstanceV,nullptr);
            ShowWindow(this->hWnd,SW_SHOW);
            UpdateWindow(this->hWnd);
        }
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    public:
        __nexusWindow() = delete;
        __nexusWindow(__nexusWindow&) = delete;
        __nexusWindow(__nexusWindow&&) = delete;
        __nexusWindow& operator=(__nexusWindow&) = delete;
        ~__nexusWindow() {
            // send debug message "Goodbye."
            OutputDebugString(L"Goodbye.\n");
            DestroyWindow(this->hWnd);
            UnregisterClass(this->wc.lpszClassName,this->wc.hInstance);
        }
    };
    Event<unsigned long long,unsigned long long> _KeyUpEvent;
    Event<unsigned long long,unsigned long long> _KeyDownEvent;
    Event<unsigned long long,unsigned long long> _KeyCharEvent;
    Event<Key,long,long> _MouseUpEvent;
    Event<Key,long,long> _MouseDownEvent;
    EventCaller<unsigned long long,unsigned long long> _KeyUpCall = _KeyUpEvent();
    EventCaller<unsigned long long,unsigned long long> _KeyDownCall = _KeyDownEvent();
    EventCaller<unsigned long long,unsigned long long> _KeyCharCall = _KeyCharEvent();
    EventCaller<Key,long,long> _MouseUpCall = _MouseUpEvent();
    EventCaller<Key,long,long> _MouseDownCall = _MouseDownEvent();
    class __nexusWindow;
    class nexusWindow;
    nexusWindow* inst = nullptr;
    __nexusWindow *_w = nullptr;
    class nexusWindow 
    {
    public:
        friend class __nexusWindow;
        HWND& __getHandle() {
            return _w->hWnd;
        }
        class Exception : public nexusException
        {
        public:
            Exception(int line, const char* file, HRESULT hr) noexcept;
            const char* what() const noexcept override;
            virtual const char* getType() const noexcept;
            static std::string TranslateErrorCode(HRESULT hr) noexcept;
            HRESULT GetErrorCode() const noexcept;
            std::string GetErrorString() const noexcept;
        private:
            HRESULT hr;
        };
        nexusWindow() = delete;
        nexusWindow(nexusWindow const&)     = delete;
        void operator=(nexusWindow const&)  = delete;
        static nexusWindow* newinst(string title, Vec2 res) {
            if (inst == nullptr) inst = new nexusWindow(title,res,true);
            return inst;
        }
        static nexusWindow* getinst() {
            return inst;
        }
        Event<unsigned long long,unsigned long long>& KeyUpEvent = _KeyUpEvent;
        Event<unsigned long long,unsigned long long>& KeyDownEvent = _KeyDownEvent;
        Event<unsigned long long,unsigned long long>& KeyCharEvent = _KeyCharEvent;
        Event<Key,long,long>& MouseUpEvent = _MouseUpEvent;
        Event<Key,long,long>& MouseDownEvent = _MouseDownEvent;
        static Vec2 getMousePos() {
            POINT p;
            GetCursorPos(&p);
            return Vec2(p.x,p.y);
        }
        static std::vector<Key> getKeysDown() {
            return nexusWindow::getinst()->keysdown;
        }
    protected:
        nexusWindow(string title, Vec2 res, bool);
    private:
        std::vector<Key> keysdown;
    };
    nexusWindow::nexusWindow(string title, Vec2 res, bool) {
        std::wstring stemp = std::wstring(title.begin(), title.end());
        LPCWSTR sw = stemp.c_str();
        _w = new __nexusWindow(sw, res);
    }
    LRESULT CALLBACK __nexusWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            try {
                switch (message)
                {
                case WM_CLOSE:
                    PostQuitMessage(wParam);
                    break;
                case WM_LBUTTONDOWN:
                    if (wParam == 1) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseDownCall(LMB,pos.x,pos.y);
                    }
                case WM_MBUTTONDOWN:
                    if (wParam == 2) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseDownCall(MMB,pos.x,pos.y);
                    }            
                case WM_RBUTTONDOWN:
                    if (wParam == 3) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseDownCall(RMB,pos.x,pos.y);
                    }
                case WM_KEYDOWN:
                    if (!isInVector(nexusWindow::getinst()->keysdown,(Key)wParam)) {
                        nexusWindow::getinst()->keysdown.push_back((Key)wParam);
                        _KeyDownCall(wParam,lParam);
                    }
                    return 0;
                case WM_LBUTTONUP:
                    if (true) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseUpCall(LMB,pos.x,pos.y);
                    }
                    if (isInVector(nexusWindow::getinst()->keysdown, Key::LMB)) {
                        nexusWindow::getinst()->keysdown.erase(std::remove(nexusWindow::getinst()->keysdown.begin(), nexusWindow::getinst()->keysdown.end(), Key::LMB), nexusWindow::getinst()->keysdown.end());
                        _KeyUpCall(Key::LMB,lParam);
                    }
                    return 0;
                case WM_MBUTTONUP:
                    if (true) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseUpCall(MMB,pos.x,pos.y);
                    }
                    if (isInVector(nexusWindow::getinst()->keysdown, Key::MMB)) {
                        nexusWindow::getinst()->keysdown.erase(std::remove(nexusWindow::getinst()->keysdown.begin(), nexusWindow::getinst()->keysdown.end(), Key::MMB), nexusWindow::getinst()->keysdown.end());
                        _KeyUpCall(Key::MMB,lParam);
                    }
                    return 0;
                case WM_RBUTTONUP:
                    if (true) {
                        POINT pos;
                        GetCursorPos(&pos);
                        _MouseUpCall(RMB,pos.x,pos.y);
                    }
                    if (isInVector(nexusWindow::getinst()->keysdown, Key::RMB)) {
                        nexusWindow::getinst()->keysdown.erase(std::remove(nexusWindow::getinst()->keysdown.begin(), nexusWindow::getinst()->keysdown.end(), Key::RMB), nexusWindow::getinst()->keysdown.end());
                        _KeyUpCall(Key::RMB,lParam);
                    }
                    return 0;
                case WM_KEYUP:
                    if (isInVector(nexusWindow::getinst()->keysdown,(Key)wParam)) {
                        nexusWindow::getinst()->keysdown.erase(std::remove(nexusWindow::getinst()->keysdown.begin(), nexusWindow::getinst()->keysdown.end(), (Key)wParam), nexusWindow::getinst()->keysdown.end());
                        _KeyUpCall(wParam,lParam);
                    }
                    return 0;
                case WM_CHAR:
                    _KeyCharCall(wParam,lParam);
                    return 0;
                }
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            catch (const nexusException& e) {
                std::string ewhatst = e.what();
                std::wstring ewhat = std::wstring(ewhatst.begin(), ewhatst.end());
                std::string etypest = e.GetType();
                std::wstring etype = std::wstring(etypest.begin(), etypest.end());
                MessageBox(nullptr, ewhat.c_str(), etype.c_str(), MB_OK | MB_ICONERROR);
            }
            catch (const std::exception& e) {
                std::string ewhatst = e.what();
                std::wstring ewhat = std::wstring(ewhatst.begin(), ewhatst.end());
                MessageBox(nullptr, ewhat.c_str(), L"Std exception", MB_OK | MB_ICONERROR);
            }
            catch (...) {
                MessageBox(nullptr, L"No details available.", L"Unknown error", MB_OK | MB_ICONERROR);
            }
            return -1;
        };
    nexusWindow::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
        :
        nexusException(line,file),
        hr(hr)
    {}
    const char* nexusWindow::Exception::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() <<std::endl
            << "[Description] " << GetErrorString() << std::endl
            << GetOrigin();
        whatBuffer = oss.str();
        return whatBuffer.c_str();
    }
    const char* nexusWindow::Exception::getType() const noexcept
    {
        return "nxE Window Exception";
    }
    std::string nexusWindow::Exception::TranslateErrorCode(HRESULT hr) noexcept
    {
        wchar_t* pMsgBuf = nullptr;
        DWORD nMsgLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&pMsgBuf),
            0,
            nullptr
        );
        if (nMsgLen == 0)
        {
            return "undefined";
        }
        std::wstring ErrMsg = pMsgBuf;
        LocalFree(pMsgBuf);
        std::string ErrMsgStd = std::string(ErrMsg.begin(), ErrMsg.end());
        return ErrMsgStd;
    }
    HRESULT nexusWindow::Exception::GetErrorCode() const noexcept
    {
        return hr;
    }
    std::string nexusWindow::Exception::GetErrorString() const noexcept
    {
        return TranslateErrorCode(GetErrorCode());
    }
}
using __windowprototype::nexusWindow;
#define nxEWnd_Except(hr) nexusWindow::Exception(__LINE__,__FILE__,hr)
/*namespace __windowprototype2 {
    class nexusWindow
    {
    public:
        class WndCls 
        {
        public:
            static const wchar_t* GetName() noexcept;
            static HINSTANCE GetInstance() noexcept;
        private:
            WndCls() noexcept;
            ~WndCls();
            WndCls(WndCls const&) = delete;
            WndCls& operator=(WndCls const&) = delete;
            static constexpr const wchar_t* wndClassName = L"nexusEngine";
            static WndCls wndClass;
            HINSTANCE hInstance;
        };
    public:
        nexusWindow(int width, int height, const wchar_t* name) noexcept;
        ~nexusWindow();
        nexusWindow(nexusWindow const&) = delete;
        nexusWindow& operator=(nexusWindow const&) = delete;
    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        int width;
        int height;
        HWND handle;
    };
    nexusWindow::WndCls::WndCls() noexcept
        :
        hInstance(GetModuleHandle(nullptr))
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = HandleMsgSetup;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetInstance();
        wc.hIcon = nullptr;
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = GetName();
        wc.hIconSm = nullptr;
        RegisterClassEx(&wc);
    }
    nexusWindow::WndCls::~WndCls()
    {
        UnregisterClass(GetName(), GetInstance());
    }
    const wchar_t* nexusWindow::WndCls::GetName() noexcept
    {
        return wndClassName;
    }
    HINSTANCE nexusWindow::WndCls::GetInstance() noexcept
    {
        return wndClass.hInstance;
    }
    nexusWindow::nexusWindow(int width, int height, const wchar_t* name) noexcept
        :
        width(width),
        height(height)
    {
        RECT wr;
        wr.left = 100;
        wr.right = wr.left + width;
        wr.top = 100;
        wr.bottom = wr.top + height;
        handle = CreateWindowExW(
            0,
            WndCls::GetName(),
            name,
            (DWORD)(WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU),
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left,
            wr.bottom - wr.top,
            nullptr,
            nullptr,
            WndCls::GetInstance(),
            this
        );
        ShowWindow(handle, SW_SHOW);
        UpdateWindow(handle);
    }
    nexusWindow::~nexusWindow()
    {
        DestroyWindow(handle);
    }
    LRESULT WINAPI nexusWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        if (msg == WM_NCCREATE)
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            nexusWindow* const pWnd = static_cast<nexusWindow*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&nexusWindow::HandleMsgThunk));
            return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    LRESULT WINAPI nexusWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch(msg)
        {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}*/
