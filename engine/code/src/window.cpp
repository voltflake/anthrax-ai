
#include "anthraxAI/vkengine.h"
#include        <chrono>
#include 		<ctime>

long long Engine::getcurtime() {
#ifdef AAI_WINDOWS
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);
    long long tim = t.tv_sec * 1000 + t.tv_usec / 1000;
	return (tim);
#endif
}

double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

void Engine::start() {
	ASSERT(state != INIT_ENGINE, "How is it possible?");
	init();
	state = ENGINE_EDITOR;
}

#if defined(AAI_WINDOWS)
void Engine::wininitwindow() {
	
	hwnd = nullptr;
	hinstance = GetModuleHandleA(nullptr);

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style =  CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hinstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = TEXT("35");

	ASSERT(!RegisterClassEx(&wcex), "Can't register winClass!");
	
	hwnd = CreateWindow(wcex.lpszClassName, TEXT("35"), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT,
            WindowExtend.width, WindowExtend.height,
            nullptr,
            nullptr,
            hinstance,
            nullptr);
	ASSERT(!hwnd, "Can't create window!");

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
}

void Engine::eventhandler(float delta)
{
	EditorCamera.checkmovement(delta);

	if (GetAsyncKeyState(VK_LBUTTON) < 0 && (Mouse.state == MOUSE_PRESSED || Mouse.state == MOUSE_SELECTED)) {
		POINT p;
		if (GetCursorPos(&p)) {
			Mouse.pos = {p.x, p.y};
		}
// printf ("Mouse position: %d | %d |\n",Mouse.posdelta.x, Mouse.posdelta.y );
		Mouse.posdelta.x = Mouse.begin.x - Mouse.pos.x;
		Mouse.posdelta.y = Mouse.begin.y - Mouse.pos.y;
		Mouse.begin = Mouse.pos;
		
		if (state & PLAY_GAME) {
			EditorCamera.checkdirection(Mouse.posdelta, delta);
		}
	}
	else {
		POINT p;
		if (GetCursorPos(&p)) {
			Mouse.pos = {p.x, p.y};
		}
	}
	if (GetAsyncKeyState(ENTER_KEY) < 0) {
		processtextind();
	}
	if (GetAsyncKeyState(ESC_KEY) & 0x01) {
		state ^= PLAY_GAME;
		state |= ENGINE_EDITOR;
	}
	if (GetAsyncKeyState(D_KEY) < 0) {
		Level.getplayer()->state |= MOVE_RIGHT;
	}
	if (GetAsyncKeyState(W_KEY) < 0) {
		Level.getplayer()->state |= MOVE_UP;
	}
	if (GetAsyncKeyState(A_KEY) < 0) {
		Level.getplayer()->state |= MOVE_LEFT;
	}
	if (GetAsyncKeyState(S_KEY) < 0) {
		Level.getplayer()->state |= MOVE_DOWN;
	}

}

VkExtent2D winext;

void Engine::runwindows() {
	MSG msg = {};

	state |= ENGINE_EDITOR;
	long long start, end = 0;
	float delta;
	while (running) {
		start = getcurtime();

		if (PeekMessage(&msg, NULL, 0,0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break ;
			if (msg.message == WM_LBUTTONDOWN) {
				Mouse.state = MOUSE_PRESSED;
				Mouse.begin = Mouse.pos;
			}
			if (msg.message == WM_LBUTTONUP) {
				Mouse.state = MOUSE_RELEASED;
				gizmomove.axis = AXIS_UNDEF;
			}
		}
		eventhandler(delta);
		if (winprepared && (winext.width != WindowExtend.width || winext.height != WindowExtend.height)) {
			WindowExtend = winext;
			Builder.resizewindow(winprepared, WindowExtend, Level.check);
			std::cout << "window w: " << WindowExtend.width << " && h: " << WindowExtend.height << '\n';
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		fpsoverlay();
		loop();

		end = getcurtime();
    	if(end - start > 0) {
        	Debug.fps = CLOCKS_PER_SEC / (end - start);
			delta = float(double(end - start)) / CLOCKS_PER_SEC;
		}
	}
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break ;
		case WM_SIZE: {
			RECT rect;
			if(GetWindowRect(hWnd, &rect)) {
				winext.width = rect.right - rect.left;
				winext.height = rect.bottom - rect.top;
			}
			break;
		}
		case WM_PAINT:
			ValidateRect(hWnd, NULL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}

	return EXIT_SUCCESS;
}
#endif

#ifdef AAI_LINUX
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

bool Engine::eventhandler(const xcb_generic_event_t *event, float delta)
{
	switch (event->response_type & ~0x80) {
		case XCB_KEY_PRESS: {
			xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        	xcb_keysym_t k = xcb_key_press_lookup_keysym(KeySyms, e, 0);

			EditorCamera.checkmovement(k, delta);

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
			if (k == MINUS_KEY) {
				Debug.boneID = Debug.boneID < 0 ? 0 : Debug.boneID - 1;
			}
			if (k == PLUS_KEY) {
				Debug.boneID = Debug.boneID > 100 ? 100 : Debug.boneID + 1;
			}
			return true;
		}
		case XCB_KEY_RELEASE: {
        	Level.getplayer()->state = IDLE;
		}
		case XCB_MOTION_NOTIFY: {
			xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;

			if (Mouse.state == MOUSE_MOVE) {
				Mouse.pos.x = motion->event_x;
            	Mouse.pos.y = motion->event_y;
				//printf ("Mouse position: %d | %d |\n", motion->event_x, motion->event_y );
			}
			if (Mouse.state == MOUSE_PRESSED || Mouse.state == MOUSE_SELECTED) {
				Mouse.pos.x = motion->event_x;
            	Mouse.pos.y = motion->event_y;
				Mouse.posdelta.x = (Mouse.begin.x - motion->event_x);
            	Mouse.posdelta.y = (Mouse.begin.y - motion->event_y);
				Mouse.begin = Mouse.pos;
				if (state & PLAY_GAME) {
					EditorCamera.checkdirection(Mouse.posdelta, delta);
				}
				//printf ("Mouse delta: %d | %d |\n", Mouse.posdelta.x, Mouse.posdelta.y );
			}
			return true;
		}
	  	case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
            if (e->detail == 1) {
            	Mouse.pos.x = e->event_x;
            	Mouse.pos.y = e->event_y;
	            Mouse.begin = Mouse.pos;
				Mouse.state = MOUSE_PRESSED;
            	std::cout <<  e->event_x << "|press|"<< e->event_y << '\n';
            }
           	return true;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
            if (e->detail == 1 && Level.check2) {
                Mouse.pos.x = 0;
            	Mouse.pos.y = 0;
            }
			else if (e->detail == 1) {
				Mouse.pos.x = e->event_x;
            	Mouse.pos.y = e->event_y;
				Mouse.state = MOUSE_RELEASED;
				gizmomove.axis = AXIS_UNDEF; 
            	std::cout <<  Mouse.pos.x << "|release|"<< Mouse.pos.y << '\n';
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

void Engine::checkstate(float delta) {
	xcb_generic_event_t *event;
	while ((event = xcb_poll_for_event(connection))) {
		ImGui_ImplX11_Event(event);
		eventhandler(event, delta);
		free(event);
	}
}

void Engine::runlinux() {
	
	xcb_flush(connection);

	state |= ENGINE_EDITOR;
	long long start, end = 0;
	float delta;

	while (running) {
		start = getcurtime();

		checkstate(delta);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplX11_NewFrame();
		ImGui::NewFrame();

		fpsoverlay();
		loop();

		if (state & EXIT) {
			xcb_key_symbols_free(KeySyms);
		}

		end = getcurtime();
    	if(end - start > 0) {
        	Debug.fps = CLOCKS_PER_SEC / (end - start);
			delta = float(double(end - start)) / CLOCKS_PER_SEC * 5000;
			//printf("delta: %f \n\n\n", delta);
		}
	}
}
#endif

