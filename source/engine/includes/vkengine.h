#pragma once

#include "vkdefines.h"
#include "vkbuilder.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkpipeline.h"

#include "levelmanager.h"

#include "vkdebug.h"

#define MAX_FPS 60
#define FPS_SAMPLER 100

#ifdef OS_WINDOWS
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

enum EngineState {
	INIT_ENGINE 	= 1 << 0,	/* 0000 0001 */
	ENGINE_EDITOR 	= 1 << 1, 	/* 0000 0010 */
	HANDLE_EVENT 	= 1 << 2,	/* 0000 0100 */
	NEW_LEVEL 		= 1 << 3,	/* 0000 1000 */
	LOAD_LEVEL 		= 1 << 4,	/* 0001 0000 */
	PLAY_GAME 		= 1 << 5,	/* 0010 0000 */
	PAUSE_GAME  	= 1 << 6,	/* 0100 0000 */
	EXIT  			= 1 << 7	/* 1000 0000 */
};

class Engine {

public:

#ifdef OS_WINDOWS
	HWND						hwnd;
	HINSTANCE					hinstance;
#endif
#ifdef OS_LINUX
	xcb_connection_t* 			connection;
	xcb_window_t 				window;
	xcb_screen_t* 				screen;
	xcb_atom_t 					wmProtocols;
	xcb_atom_t 					wmDeleteWin;
	xcb_key_symbols_t   		*KeySyms;
#endif
	int							state = INIT_ENGINE;

	void 						start();
	void 						checkstate();
void checkuistate();
	void 						initengine(LevelManager &levels);

	VkBuilder 					Builder;
	LevelManager				Level;

	std::unordered_map<int, Data> resources;

	VkExtent2D 					WindowExtend = {800, 800};
	bool 						winprepared = false;
	bool						freemove = false;

	Positions 					playerpos = {0, 0};
	Positions 					mousepos = {0, 0};
	MouseState					mousestate = MOUSE_IDLE;

	std::vector<std::string> 	checkimgs= {"1.raw", "2.raw", "3.raw", "4.raw", "5.raw", "6.raw", "7.raw", "8.raw", "9.raw", "10.raw"};
	int 						checkimg = 0;
	bool 						checkupdate = false;

#ifdef OS_WINDOWS
	void 						wininitwindow();
	void 						runwindows();
#endif
#ifdef OS_LINUX
	bool 						running = true;
	float 						fps = 0;

	void 						calculateFPS(std::chrono::duration<double, std::milli>& delta);

	void 						linuxinitwindow();
	void  						runlinux();
	bool 						eventhandler(const xcb_generic_event_t *event);
#endif
	void 						init();
	void 						cleanup();

private:
	DeletionQueue 				Deletor;
	int 						FrameIndex = 0;

	void 						moveplayer();
	void 						catchobject();
	void 						uncatchobject();
	bool 						editormove();
	bool 						collision(int& state, bool collision, Positions pos);

	void 						draw();
	void 						drawobjects(VkCommandBuffer cmd,RenderObject* first, int count);
	
	void  						ui();
	void  						fpsoverlay();

	void 						initscene();
	void 						initvulkan();
	void 						initimgui();

	void 						initresources();
	void 						reloadresources();

	void 						loadmylevel();
};
