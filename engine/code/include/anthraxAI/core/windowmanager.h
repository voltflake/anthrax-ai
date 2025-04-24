#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"

#include "anthraxAI/engine.h"

#include <cstring>
#ifdef __linux__
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_keysyms.h>

static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t *connection, bool onlyifexist, const char *str)
{
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, onlyifexist, strlen(str), str);
	return xcb_intern_atom_reply(connection, cookie, NULL);
}
#elif defined(_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include <backends/imgui_impl_win32.h>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

#define MAX_FPS 60

enum WindowEvents {
    WINDOW_EVENT_RUN            = 1 << 0,	/* 0000 0001 */
    WINDOW_EVENT_KEY_PRESSED    = 1 << 1, 	/* 0000 0010 */
    WINDOW_EVENT_KEY_RELEASED   = 1 << 2, 	/* 0000 0100 */
    WINDOW_EVENT_MOUSE_MOVE     = 1 << 3,
    WINDOW_EVENT_MOUSE_PRESSED  = 1 << 4,
    WINDOW_EVENT_MOUSE_RELEASED = 1 << 5,
    WINDOW_EVENT_EXIT           = 1 << 6,
    WINDOW_EVENT_RESIZE         = 1 << 7,
};

namespace Core
{
    struct MouseInfo {
        Vector2<int> Position = { 0, 0};
        Vector2<int> Delta = { 0, 0};
        Vector2<int> Begin = { 0, 0};
        Vector2<int> BeginPress = { 0, 0};
        Vector2<int> Event = { 0, 0};
        bool Pressed = false;
        bool Selected = false;
    };

    class WindowManager : public Utils::Singleton<WindowManager>
    {
#ifdef __linux__
        private:
            xcb_connection_t* 			Connection;
            xcb_window_t 				Window;
            xcb_screen_t* 				Screen;
            xcb_atom_t 					WMProtocols;
            xcb_atom_t 					WMDeleteWin;
            xcb_key_symbols_t   		*KeySymbols;
            xcb_keysym_t                PressedKey;
        public:
            void InitLinuxWindow();
            void RunLinux();

            xcb_connection_t* GetConnection() const { return Connection; }
            xcb_window_t* GetWindow() { return &Window; }
#elif defined(_WIN32)
        public:
            void InitWindowsWindow();
            void RunWindows();

            HWND GetWinWindow() { return Hwnd; }
            HINSTANCE GetWinInstance() { return Hinstance; }
#endif
        public:
            Vector2<int> GetScreenResolution() const { return Extents; }

            Vector2<int> GetMousePos() const { return Mouse.Position; }
            Vector2<int> GetMouseBegin() const { return Mouse.Begin; }
            Vector2<int> GetMouseBeginPress() const { return Mouse.BeginPress; }
            Vector2<int> GetMouseDelta() const { return Mouse.Delta; }
            bool IsMousePressed() const { return Mouse.Pressed; }
            bool IsMouseSelected() const { return Mouse.Selected; }
            void ReleaseMouseSelected() { Mouse.Selected = false;}
            void SetResizeExtents(int x, int y) { OnResizeExtents.x = x; OnResizeExtents.y = y; }
            void SetEvent(int event) { Event |= event; }
#ifdef __linux__
            xcb_keysym_t GetPressedKey() const { return PressedKey; }
#elif defined(_WIN32)
        private:
            int PressedKey;
            HWND Hwnd;
	        HINSTANCE Hinstance;
        public:
            int GetPressedKey() const { return PressedKey; }
#endif
        private:
            int Event;
            MouseInfo Mouse;
            Vector2<int> Extents = { 1000, 1000 };
            Vector2<int> OnResizeExtents;

            void Events();
            void ProcessEvents();
#ifdef __linux__
            int CatchEvent(xcb_generic_event_t *event);
#elif defined(_WIN32)
            int CatchEvent();
#endif
            bool running = true;
    };
}
