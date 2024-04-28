
#include "../includes/vkengine.h"
#include        <chrono>
#include 		<ctime>

#ifdef OS_WINDOWS
void Engine::wininitwindow() {
	
	hwnd = nullptr;
	hinstance = GetModuleHandleA(nullptr);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style =  CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_SLIDESHOW));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SLIDESHOW_SM));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = TEXT("MyTestClass");

	ASSERT(!RegisterClassEx(&wcex), "Can't register winClass!");
	
	hwnd = CreateWindow(wcex.lpszClassName, TEXT("35"),WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
            CW_USEDEFAULT, CW_USEDEFAULT,
            WindowExtend.width, WindowExtend.height,
            nullptr,
            nullptr,
            hinstance,
            nullptr);
	ASSERT(!hwnd, "Can't create window!");
}

void Engine::runwindows() {

    ShowWindow(hwnd, SW_SHOWNORMAL);
	MSG msg = {};
	
	static int ticktrigger = 0;
	int tickcount;
    
    // main loop
	while (true) {
		if (PeekMessage(&msg, NULL, 0,0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break ;
		}
       tickcount = GetTickCount();
       if (tickcount > ticktrigger) {
       		ticktrigger = tickcount + 10;
       		draw();
       }
	}
   	//vkDeviceWaitIdle(Device);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break ;
		 case WM_SIZE:
	    {
	        UINT width = LOWORD(lParam);
	        UINT height = HIWORD(lParam);
	        if (width == SIZE_MINIMIZED || height == SIZE_MINIMIZED) {
	        	ShowWindow(hWnd, SW_HIDE);
	        }
	        
	    }
	    	return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}

	return 0;
}
#endif

#ifdef OS_LINUX

void Engine::linuxinitwindow() {
	uint32_t value_mask, value_list[32];

	int screenp = 0;

	connection = xcb_connect(NULL, &screenp);
	if (xcb_connection_has_error(connection)){
		throw std::runtime_error("Error: Failed to connect to X server using XCB");

	}
	window = xcb_generate_id(connection);

	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

	for (int s = screenp; s > 0; s--) {
	    xcb_screen_next(&iter);
	}
	screen = iter.data;

	value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	value_list[0] = screen->black_pixel;
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
		WindowExtend.width = screen->width_in_pixels;
		WindowExtend.height = screen->height_in_pixels;
	}

	xcb_create_window(connection,
		XCB_COPY_FROM_PARENT,
		window, screen->root,
		0, 0, WindowExtend.width, WindowExtend.height, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual,
		value_mask, value_list);

	xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
    connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
	xcb_intern_atom_cookie_t wmProtocolsCookie =
	    xcb_intern_atom(connection, 1, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
	xcb_intern_atom_reply_t *wmDeleteReply =
	    xcb_intern_atom_reply(connection, wmDeleteCookie, NULL);
	xcb_intern_atom_reply_t *wmProtocolsReply =
	    xcb_intern_atom_reply(connection, wmProtocolsCookie, NULL);
	wmDeleteWin = wmDeleteReply->atom;
	wmProtocols = wmProtocolsReply->atom;

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
	wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);


	std::string windowtitle = "35";
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE,
	window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
	windowtitle.size(), windowtitle.c_str());

	std::string wm_class;
	std::string name = "name";
	wm_class = wm_class.insert(0, name);
	wm_class = wm_class.insert(name.size(), 1, '\0');
	wm_class = wm_class.insert(name.size() + 1, windowtitle);
	wm_class = wm_class.insert(wm_class.size(), 1, '\0');
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, wm_class.size() + 2, wm_class.c_str());

	if (fullscreen) {
		xcb_intern_atom_reply_t *atom_wm_state = intern_atom_helper(connection, false, "_NET_WM_STATE");
		xcb_intern_atom_reply_t *atom_wm_fullscreen = intern_atom_helper(connection, false, "_NET_WM_STATE_FULLSCREEN");
		xcb_change_property(connection,
		XCB_PROP_MODE_REPLACE,
		window, atom_wm_state->atom,
		XCB_ATOM_ATOM, 32, 1,
		&(atom_wm_fullscreen->atom));
		free(atom_wm_fullscreen);
		free(atom_wm_state);
	}

	xcb_map_window(connection, window);
	xcb_flush(connection);
	KeySyms = xcb_key_symbols_alloc(connection);
}

bool Engine::eventhandler(const xcb_generic_event_t *event)
{
	switch (event->response_type & ~0x80) {
		case XCB_KEY_PRESS: {
			xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        	xcb_keysym_t k = xcb_key_press_lookup_keysym(KeySyms, e, 0);
        	// if (k == ENTER_KEY && checkimg < checkimgs.size() - 1) {
        	// 	checkupdate = true;
        	// }
			if (k == ENTER_KEY) {
				processtextind();
			}
			if (k == ESC_KEY) {
        		state ^= PLAY_GAME;
				state |= ENGINE_EDITOR;
        	}
        	if (k == D_KEY) {
				Level.getplayer()->state |= MOVE_RIGHT;
        	}
        	if (k == W_KEY) {
				Level.getplayer()->state |= MOVE_UP;
        	}
        	if (k == A_KEY) {
				Level.getplayer()->state |= MOVE_LEFT;
        	}
        	if (k == S_KEY) {
				Level.getplayer()->state |= MOVE_DOWN;
        	}
			return true;
		}
		case XCB_KEY_RELEASE: {
        	Level.getplayer()->state = IDLE;
		}
		case XCB_MOTION_NOTIFY: {
			xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;
			if (mousestate == MOUSE_MOVE) {
				mousepos.x = motion->event_x;
            	mousepos.y = motion->event_y;
				//printf ("Mouse position: %d | %d |\n", motion->event_x, motion->event_y );
			}
			return true;
		}
	  	case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
            if (e->detail == 1) {
            	mousepos.x = e->event_x;
            	mousepos.y = e->event_y;
	            mousebegin = mousepos;
				mousestate = MOUSE_PRESSED;
            	std::cout <<  e->event_x << "|press|"<< e->event_y << '\n';
            }
           	return true;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
            if (e->detail == 1 && Level.check2) {
                mousepos.x = 0;
            	mousepos.y = 0;
            }
			else if (e->detail == 1) {
				mousepos.x = e->event_x;
            	mousepos.y = e->event_y;
				mousestate = MOUSE_RELEASED;
            	std::cout <<  mousepos.x << "|release|"<< mousepos.y << '\n';
			}
           	return true;
        }
		case XCB_EXPOSE: {
		    xcb_flush(connection);
		    return true;
		}
		case XCB_DESTROY_NOTIFY:
			state |= EXIT;
			running = false;
		    return true;
		break;
		case XCB_CLIENT_MESSAGE: {
		    if(((xcb_client_message_event_t*)event)->data.data32[0] == wmDeleteWin) {
				state |= EXIT;
		    	running = false;
			}
		    return true;
		}
		case XCB_CONFIGURE_NOTIFY: {
			const xcb_configure_notify_event_t *cfgEvent = (const xcb_configure_notify_event_t *)event;
			if ((winprepared) && ((cfgEvent->width != WindowExtend.width) || (cfgEvent->height != WindowExtend.height))) {
				WindowExtend.width = cfgEvent->width;
				WindowExtend.height = cfgEvent->height;
				if ((WindowExtend.width > 0) && (WindowExtend.height > 0)) {
					Builder.resizewindow(winprepared, WindowExtend, Level.check);
					std::cout << "window w: " << WindowExtend.width << " && h: " << WindowExtend.height << '\n';
				}
			}
		}
		default:
            return false;
	}
	return false;
}

void Engine::calculateFPS(std::chrono::duration<double, std::milli>& delta) {

    if (delta.count() < (1000.0f / MAX_FPS + 2.0f / MAX_FPS)) {
        std::chrono::duration<double, std::milli> deltams((1000.0f / MAX_FPS) + (2.0f / MAX_FPS) - delta.count());
        auto msduration = std::chrono::duration_cast<std::chrono::milliseconds>(deltams);
        std::this_thread::sleep_for(std::chrono::milliseconds(msduration.count()));
        fps = 1000.0f / deltams.count();
    }
}

void Engine::checkstate() {
	xcb_generic_event_t *event;
	while ((event = xcb_poll_for_event(connection))) {
		ImGui_ImplX11_Event(event);
		eventhandler(event);
		free(event);
	}
}

void Engine::runlinux() {
	
	xcb_flush(connection);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	state |= ENGINE_EDITOR;

	while (running) {
		checkstate();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplX11_NewFrame();
		ImGui::NewFrame();
		
		fpsoverlay();

		if (state & PLAY_GAME) {
			start = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> delta = start - end;
			calculateFPS(delta);

			processanimation();
			processtrigger();
			moveplayer();
			draw();
			end = std::chrono::system_clock::now();
		}
		if (state & ENGINE_EDITOR) {
			start = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> delta = start - end;
			calculateFPS(delta);
			
			ui();
			catchobject();
			editormove();
			uncatchobject();
			draw();
			end = std::chrono::system_clock::now();
		}
		if (state & EXIT) {
			xcb_key_symbols_free(KeySyms);
			cleanup();
		}
	
	}
}
#endif