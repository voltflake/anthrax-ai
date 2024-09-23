#include "anthraxAI/core/windowmanager.h"

#include "anthraxAI/core/scene.h"

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

WindowEvents Core::WindowManager::CatchEvent(xcb_generic_event_t *event)
{
    switch (event->response_type & ~0x80) {
		case XCB_KEY_PRESS:
			return WINDOW_EVENT_KEY_PRESSED;
		case XCB_KEY_RELEASE:
        	return WINDOW_EVENT_KEY_PRESSED;
		case XCB_MOTION_NOTIFY:
        	return WINDOW_EVENT_MOUSE_MOVE;
	  	case XCB_BUTTON_PRESS:
            return WINDOW_EVENT_MOUSE_PRESSED;
        case XCB_BUTTON_RELEASE:
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
		case XCB_CONFIGURE_NOTIFY:
            return WINDOW_EVENT_RESIZE;
        default:
            break;
	}
	return WINDOW_EVENT_RUN;
}

void Core::WindowManager::ProcessEvents()
{
    if (Event & WINDOW_EVENT_EXIT) {
        Engine::GetInstance()->SetState(ENGINE_STATE_EXIT);
        running = false;
    }
}

void Core::WindowManager::Events()
{
	xcb_generic_event_t *event;
	while ((event = xcb_poll_for_event(Connection))) {
		//ImGui_ImplX11_Event(event);
		Event = CatchEvent(event);
		free(event);
	}
    ProcessEvents();
}

void Core::WindowManager::RunLinux()
{
    xcb_flush(Connection);

	while (running) {
        Events();

		Core::Scene::GetInstance()->RenderScene();

		if (Engine::GetInstance()->GetState() & ENGINE_STATE_EXIT) {
			xcb_key_symbols_free(KeySymbols);
		}
	}
}
#endif