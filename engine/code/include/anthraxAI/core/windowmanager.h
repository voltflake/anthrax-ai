#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"

#include "anthraxAI/engine.h"

#include <cstring>
#ifdef AAI_LINUX
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_keysyms.h>

static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t *connection, bool onlyifexist, const char *str)
{
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, onlyifexist, strlen(str), str);
	return xcb_intern_atom_reply(connection, cookie, NULL);
}

#include <imgui_impl_x11.h>
static ImGui_ImplVulkanH_Window MainWindowData;
#endif

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
    class WindowManager : public Utils::Singleton<WindowManager>
    {
#ifdef AAI_LINUX
        private:
            xcb_connection_t* 			Connection;
            xcb_window_t 				Window;
            xcb_screen_t* 				Screen;
            xcb_atom_t 					WMProtocols;
            xcb_atom_t 					WMDeleteWin;
            xcb_key_symbols_t   		*KeySymbols;

        public:
            void InitLinuxWindow();
            void RunLinux();

            xcb_connection_t* GetConnection() const { return Connection; }
            xcb_window_t GetWindow() const { return Window; }
#else
        public:
            void InitWindowsWindow();
#endif
        public:
            void InitImGui();
            void ViewEditor();

            Vector2<int> GetScreenResolution() const { return Extents; }

        private:
            WindowEvents Event;
            Vector2<int> Extents = { 1000, 800 };

            void Events();
            void ProcessEvents();
            WindowEvents CatchEvent(xcb_generic_event_t *event);
            ImGuiStyle 	EditorStyle;

            bool running = true;
    };
}
