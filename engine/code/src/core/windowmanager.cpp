#include "anthraxAI/core/windowmanager.h"

#include "anthraxAI/core/scene.h"
#include "anthraxAI/core/imguihelper.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/utils/defines.h"
#include <ctime>
#include <iostream>

#ifdef AAI_LINUX
void Core::WindowManager::InitLinuxWindow()
{
    uint32_t value_mask, value_list[32];
	int screenp = 0;
	Connection = xcb_connect(NULL, &screenp);
	if (xcb_connection_has_error(Connection)){
		ASSERT(true, "Error: Failed to connect to X server using XCB");
	}
	Window = xcb_generate_id(Connection);

	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(Connection));
	for (int s = screenp; s > 0; s--) {
	    xcb_screen_next(&iter);
	}
	Screen = iter.data;

	value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	value_list[0] = Screen->black_pixel;
	value_list[1] =
		XCB_EVENT_MASK_KEY_RELEASE |
		XCB_EVENT_MASK_KEY_PRESS |
		XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY |
		XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE;

	bool fullscreen = false;
	if (fullscreen) {
		Extents.x = Screen->width_in_pixels;
		Extents.y = Screen->height_in_pixels;
	}

	xcb_create_window(Connection,
		XCB_COPY_FROM_PARENT,
		Window, Screen->root,
		0, 0, Extents.x, Extents.y, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		Screen->root_visual,
		value_mask, value_list);

	xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
    Connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
	xcb_intern_atom_cookie_t wmProtocolsCookie =
	    xcb_intern_atom(Connection, 1, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
	xcb_intern_atom_reply_t *wmDeleteReply =
	    xcb_intern_atom_reply(Connection, wmDeleteCookie, NULL);
	xcb_intern_atom_reply_t *wmProtocolsReply =
	    xcb_intern_atom_reply(Connection, wmProtocolsCookie, NULL);
	WMDeleteWin = wmDeleteReply->atom;
	WMProtocols = wmProtocolsReply->atom;

	xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, Window,
	wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);

	std::string windowtitle = "35";
	xcb_change_property(Connection, XCB_PROP_MODE_REPLACE,
	Window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
	windowtitle.size(), windowtitle.c_str());

	std::string wm_class;
	std::string name = "name";
	wm_class = wm_class.insert(0, name);
	wm_class = wm_class.insert(name.size(), 1, '\0');
	wm_class = wm_class.insert(name.size() + 1, windowtitle);
	wm_class = wm_class.insert(wm_class.size(), 1, '\0');
	xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, Window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, wm_class.size() + 2, wm_class.c_str());

	if (fullscreen) {
		xcb_intern_atom_reply_t *atom_wm_state = intern_atom_helper(Connection, false, "_NET_WM_STATE");
		xcb_intern_atom_reply_t *atom_wm_fullscreen = intern_atom_helper(Connection, false, "_NET_WM_STATE_FULLSCREEN");
		xcb_change_property(Connection,
		XCB_PROP_MODE_REPLACE,
		Window, atom_wm_state->atom,
		XCB_ATOM_ATOM, 32, 1,
		&(atom_wm_fullscreen->atom));
		free(atom_wm_fullscreen);
		free(atom_wm_state);
	}

	xcb_map_window(Connection, Window);
	xcb_flush(Connection);
	KeySymbols = xcb_key_symbols_alloc(Connection);
}

int Core::WindowManager::CatchEvent(xcb_generic_event_t *event)
{
    switch (event->response_type & ~0x80) {
		case XCB_KEY_PRESS: {
			xcb_key_press_event_t* e = (xcb_key_press_event_t*)event;
        	PressedKey = xcb_key_press_lookup_keysym(KeySymbols, e, 0);
			return WINDOW_EVENT_KEY_PRESSED;
		}
		case XCB_KEY_RELEASE:
            PressedKey = -1;
        	return WINDOW_EVENT_KEY_RELEASED;
		case XCB_MOTION_NOTIFY: {
			xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
			Mouse.Event = { motion->event_x, motion->event_y };
        	return WINDOW_EVENT_MOUSE_MOVE;
		}
	  	case XCB_BUTTON_PRESS: {		
            xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
			Mouse.Event = { motion->event_x, motion->event_y };
            return WINDOW_EVENT_MOUSE_PRESSED;
        }
        case XCB_BUTTON_RELEASE:
			Mouse.Pressed = false;
            return WINDOW_EVENT_MOUSE_RELEASED;
		case XCB_EXPOSE:
		    xcb_flush(Connection);
		    break;
		case XCB_DESTROY_NOTIFY:
            return WINDOW_EVENT_EXIT;
		case XCB_CLIENT_MESSAGE:
		    if(((xcb_client_message_event_t*)event)->data.data32[0] == WMDeleteWin) {
                return WINDOW_EVENT_EXIT;
			}
		   	break;
		case XCB_CONFIGURE_NOTIFY: {
                const xcb_configure_notify_event_t* e = (const xcb_configure_notify_event_t*)event;
                OnResizeExtents = { e->width, e->height };
                return WINDOW_EVENT_RESIZE;
            }
        default:
            break;
	}
	return WINDOW_EVENT_RUN;
}

void Core::WindowManager::ProcessEvents()
{
    if (Utils::IsBitSet(Event, WINDOW_EVENT_KEY_PRESSED)) {
        if (PressedKey == ESC_KEY) {
            Engine::GetInstance()->ToogleEditorMode();
            Utils::ClearBit(&Event, WINDOW_EVENT_KEY_PRESSED);
        }
        if (PressedKey == MINUS_KEY) {
            if (Utils::Debug::GetInstance()->BoneID > 0) {
                Utils::Debug::GetInstance()->BoneID--;
            }
        }
        if (PressedKey == PLUS_KEY) {
            Utils::Debug::GetInstance()->BoneID++;
        }

    }
    if (Utils::IsBitSet(Event, WINDOW_EVENT_KEY_RELEASED)) {
        Utils::ClearBit(&Event, WINDOW_EVENT_KEY_RELEASED);
        Utils::ClearBit(&Event, WINDOW_EVENT_KEY_PRESSED);
    }

    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_PLAY)) {
        if (Utils::IsBitSet(Event, WINDOW_EVENT_MOUSE_RELEASED)) {
            Mouse.Delta = {0, 0};
            Mouse.Pressed = false;
            Mouse.Selected = false;
            //printf("------MOUSE RELEASED--------\n");
            Utils::ClearBit(&Event, WINDOW_EVENT_MOUSE_MOVE);
            Utils::ClearBit(&Event, WINDOW_EVENT_MOUSE_RELEASED);
        }
        if (Utils::IsBitSet(Event, WINDOW_EVENT_MOUSE_PRESSED)) {
            //printf("------MOUSE PRESSED--------\n");
            Mouse.Pressed = true;
            Mouse.Selected = true;
            Mouse.Begin = Mouse.Event;
            Utils::ClearBit(&Event, WINDOW_EVENT_MOUSE_PRESSED);
        }
        if (Utils::IsBitSet(Event,  WINDOW_EVENT_MOUSE_MOVE) && Mouse.Pressed) {
            Mouse.Position.x = Mouse.Event.x;
            //printf("------MOUSE MOVE--------\n");
            Mouse.Position.y = Mouse.Event.y;
            Mouse.Delta.x = Mouse.Begin.x - Mouse.Event.x;
            Mouse.Delta.y = Mouse.Begin.y - Mouse.Event.y;
            Mouse.Begin = Mouse.Position;
        }
    }
    if (Utils::IsBitSet(Event, WINDOW_EVENT_EXIT)) {
        Engine::GetInstance()->SetState(ENGINE_STATE_EXIT);
        running = false;
    }
    if (Utils::IsBitSet(Event, WINDOW_EVENT_RESIZE)) {
        Vector2<int> tmp = Extents;
        Extents.x = OnResizeExtents.x;
        Extents.y = OnResizeExtents.y;
        if (!Engine::GetInstance()->OnResize()) {
            Extents = tmp;
        }
        Utils::ClearBit(&Event, WINDOW_EVENT_RESIZE);
    }
}

void Core::WindowManager::Events()
{
	xcb_generic_event_t *event;
	while ((event = xcb_poll_for_event(Connection))) {
		Core::ImGuiHelper::GetInstance()->CatchEvent(event);
		Event |= (CatchEvent(event));
   	    free(event);
	}
    ProcessEvents();
}

void Core::WindowManager::RunLinux()
{
    xcb_flush(Connection);

    long long start, end = 0;
    float delta = 0;
	while (running) {
        start = clock();
        Events();

        while (delta < CLOCKS_PER_SEC / MAX_FPS) {
			start = clock();
			delta = (float(start - end));
		}
        Utils::Debug::GetInstance()->FPS = CLOCKS_PER_SEC / delta;

		Core::ImGuiHelper::GetInstance()->UpdateFrame();
        Core::Scene::GetInstance()->Loop();

		if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_EXIT)) {
			xcb_key_symbols_free(KeySymbols);
		}
        
        end = clock();
        delta = (float(end - start));
        Utils::Debug::GetInstance()->DeltaMs = (end - start) / float(CLOCKS_PER_SEC) * 1000.0;
	}
}
#endif
