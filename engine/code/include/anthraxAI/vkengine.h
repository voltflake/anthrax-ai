#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkbuilder.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"

#include "anthraxAI/levelmanager.h"
#include "anthraxAI/camera.h"

#include "anthraxAI/vkdebug.h"

#if defined(AAI_WINDOWS)
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
	EXIT  			= 1 << 7,	/* 1000 0000 */
	MODE_2D  		= 1 << 8,	/* 1000 0000 0 */
	MODE_3D  		= 1 << 9,	/* 1000 0000 00*/
};

class Engine {

public:

#if defined(AAI_WINDOWS)
	HWND						hwnd;
	HINSTANCE					hinstance;
#endif
#ifdef AAI_LINUX
	xcb_connection_t* 			connection;
	xcb_window_t 				window;
	xcb_screen_t* 				screen;
	xcb_atom_t 					wmProtocols;
	xcb_atom_t 					wmDeleteWin;
	xcb_key_symbols_t   		*KeySyms;
#endif
	int							state = INIT_ENGINE;

	void 						start();
	void 						checkstate(float delta);
	void 						checkuistate();
	void 						initengine(LevelManager &levels);

	VkBuilder 					Builder;
	LevelManager				Level;
	Camera 						EditorCamera;

	std::unordered_map<int, Data> resources;

	CameraData 					camdata;
	StorageData 				storagedata;

	VkExtent2D 					WindowExtend = {1200, 800};
	bool 						winprepared = false;
	bool						freemove = false;

	Gizmo						gizmomove;

	Positions 					playerpos = {0, 0};
	Positions 					mousepos = {0, 0};
	Positions 					mouseposdelta = {0, 0};
	Positions 					mousebegin = {0, 0};
	MouseState					mousestate = MOUSE_IDLE;

	std::vector<std::string> 	checkimgs = {"1.raw", "2.raw", "3.raw", "4.raw", "5.raw", "6.raw", "7.raw", "8.raw", "9.raw", "10.raw"};
	int 						checkimg = 0;
	bool 						checkupdate = false;

#if defined(AAI_WINDOWS)	
	void 						wininitwindow();
	void 						runwindows();
	void 						eventhandler(float delta);
#endif
#ifdef AAI_LINUX
	void 						linuxinitwindow();
	void  						runlinux();
	bool 						eventhandler(const xcb_generic_event_t *event, float delta);
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

	void 						loop();

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
	void 						update3d();
	void 						preparecamerabuffer();
	void 						mousepicking();
	void 						render();
	void 						renderscene(VkCommandBuffer cmd,RenderObject* first, int count);
	
	void  						ui();
	void 						debuglight();
	void 						debug2d(bool* active);
	void 						debug3d(bool* active);
	void 						debugmouse();
	void  						fpsoverlay();

	void 						initscene();
	void 						initvulkan();
	void 						initimgui();

	void 						initresources();
	void 						reloadresources();

	void 						loadmylevel();
};

