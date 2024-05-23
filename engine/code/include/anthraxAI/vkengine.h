#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkbuilder.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"

#include "anthraxAI/levelmanager.h"

#include "anthraxAI/vkdebug.h"

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
	void 						checkuistate();
	void 						initengine(LevelManager &levels);

	VkBuilder 					Builder;
	LevelManager				Level;

	std::unordered_map<int, Data> resources;

	CameraData 					camdata;
	VkExtent2D 					WindowExtend = {800, 800};
	bool 						winprepared = false;
	bool						freemove = false;

	Positions 					playerpos = {0, 0};
	Positions 					mousepos = {0, 0};
	Positions 					mousebegin = {0, 0};
	MouseState					mousestate = MOUSE_IDLE;

	std::vector<std::string> 	checkimgs = {"1.raw", "2.raw", "3.raw", "4.raw", "5.raw", "6.raw", "7.raw", "8.raw", "9.raw", "10.raw"};
	int 						checkimg = 0;
	bool 						checkupdate = false;

#ifdef OS_WINDOWS
	void 						wininitwindow();
	void 						runwindows();
#endif
#ifdef OS_LINUX
	void 						linuxinitwindow();
	void  						runlinux();
	bool 						eventhandler(const xcb_generic_event_t *event);
#endif

	bool 						running = true;
	float 						fps = 0;
	void 						calculateFPS(std::chrono::duration<double, std::milli>& delta);

	void 						run();
	void 						init();
	void 						cleanup();

	ImGuiStyle 					EditorStyle;
	ImGuiStyle 					TextDisplayStyle;

	std::vector<DebugAnim> 		DebugImGuiAnim;

	float zoomtest = 0;
private:
	DeletionQueue 				Deletor;
	int 						FrameIndex = 0;

	void 						game_loop();

	void 						moveplayer();

	void 						processanimation();

	bool 						processtrigger();
	void 						processaction();
	void 						processtext();
	void 						processtextind();

	void 						animator();

	void 						editor();
	void 						catchobject();
	void 						uncatchobject();
	bool 						editormove();
	bool 						collision(int& state, bool collision, Positions pos, Positions sizes);

	void 						move();
	void 						update();
	void 						draw();
	void 						drawobjects(VkCommandBuffer cmd,RenderObject* first, int count);
	
	void  						ui();
	void 						debuglight();
	void 						debugdraw();
	void  						fpsoverlay();

	void 						initscene();
	void 						initvulkan();
	void 						initimgui();

	void 						initresources();
	void 						reloadresources();

	void 						loadmylevel();
};
