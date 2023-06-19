
#include "../includes/vkengine.h"
#include        <chrono>

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
// xcb_key_symbols_t   *KeySyms; // move to engine.h

void Engine::linuxinitwindow() {
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
	window = xcb_generate_id(connection);
	uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	uint32_t valueList[2] ;//= {screen->black_pixel, 0};
	valueList[0] = screen->black_pixel;
	   valueList[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_MOTION;
	xcb_create_window(
	connection,
	XCB_COPY_FROM_PARENT,
	window,
	screen->root,
	0,
	0,
	screen->width_in_pixels,
	screen->height_in_pixels,
	0,
	XCB_WINDOW_CLASS_INPUT_OUTPUT,
	screen->root_visual,
	eventMask,
	valueList);

	xcb_change_property(
	connection,
	XCB_PROP_MODE_REPLACE,
	window,
	XCB_ATOM_WM_NAME,
	XCB_ATOM_STRING,
	8,
	strlen("35"),
	"35");

	xcb_map_window(connection, window);
	xcb_flush(connection);
	KeySyms = xcb_key_symbols_alloc(connection);

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

}

uint32_t getTick() {
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}

bool Engine::handleEvent(const xcb_generic_event_t *event)
{
	switch (event->response_type & ~0x80) {
		case XCB_KEY_PRESS: {
			 xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        	xcb_keysym_t k = xcb_key_press_lookup_keysym(KeySyms, e, 0);
        	if(k == D_KEY) {
        		Levels.level.player.x += 5;

        	}
        	if(k == W_KEY) {
        		Levels.level.player.y -= 5;
        	}
        	if(k == A_KEY) {
        		Levels.level.player.x -= 5;

        	}
        	if(k == S_KEY) {
        		Levels.level.player.y += 5;
        	}
			return true;
		}
	  	case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
           	return true;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
           	return true;
        }
		case XCB_EXPOSE: {
		    xcb_flush(connection);
		    return true;
		}
		case XCB_CLIENT_MESSAGE: {
		    if(((xcb_client_message_event_t*)event)->data.data32[0] == wmDeleteWin)
		       running = false;
		    return true;
		}
		default:
            return false;
	}
	
}

void Engine::runlinux() {
	xcb_flush(connection);
	static int ticktrigger = 0;
	static float frameTimer = 1.0f;
	static float fpsTimer = 0.0f;
	static uint32_t frameCounter = 0;
	//int tickcount;

	while (running)
	{
		auto tStart = std::chrono::high_resolution_clock::now();
		xcb_generic_event_t *event;
		event = xcb_poll_for_event(connection);
		if (event)
		{
			ImGui_ImplX11_Event(event);
		 	handleEvent(event);
			free(event);
		}
		else {
				auto tEnd = std::chrono::high_resolution_clock::now();
				auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
				//ameTimer = tDiff / 1000.0f;
				//fpsTimer += (float)tDiff;

			    ImGui_ImplVulkan_NewFrame();
		        ImGui_ImplX11_NewFrame();
		        ImGui::NewFrame();

		        ui();
		       	draw();
				//draw();
				//frameCounter++;
				//auto tEnd = std::chrono::high_resolution_clock::now();
				//auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
				frameTimer = tDiff / 1000.0f;
				fpsTimer += (float)tDiff;
				if (fpsTimer > 1000.0f)
				{
					xcb_change_property(connection, XCB_PROP_MODE_REPLACE,
						window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
						strlen("35"), "35");
					lastFPS = (float)frameCounter * (1000.0f / fpsTimer);
					fpsTimer = 0.0f;
					frameCounter = 0;
				}
			}
		
	}

	vkDeviceWaitIdle(Builder.getdevice());
	ImGui_ImplVulkan_Shutdown();
    ImGui_ImplX11_Shutdown();
    ImGui::DestroyContext();
}
#endif