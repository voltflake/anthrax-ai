#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkbuilder.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"

#include "anthraxAI/animator.h"
#include "anthraxAI/levelmanager.h"
#include "anthraxAI/resourcemanager.h"
#include "anthraxAI/vkcmdhandler.h"

#include "anthraxAI/camera.h"

#include "anthraxAI/vkdebug.h"

#if defined(AAI_WINDOWS)
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

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
	int					state = INIT_ENGINE;

	void 						start();
	void 						checkstate(float delta);
	void 						checkuistate();
	void 						initengine(LevelManager &levels);

	void load2dresources();
	void load3dresources();


	VkBuilder 					Builder;
	CmdHandler 					Cmd;
	ResourceManager				Resources;
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
	
	std::chrono::duration<double, std::milli> deltatime;
	long long startms;
	Animator animator;
	bool animprepared = false;
	bool debugbones = false;
 	float animspeed = 1.0;

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

	long long 					getcurtime();

	void 						run();
	void 						init();
	void 						cleanup();

	int debugboneID = 0;

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

	void 						animator2d();

	void 						editor();
	void 						catchobject();
	void 						uncatchobject();
	bool 						editormove();
	bool 						collision(int& state, bool collision, Positions pos, Positions sizes);

	void 						move();
	void 						update();
	void 						update3d();
	void 						preparecamerabuffer();
	void 						updatebones(int id);

	void 						mousepicking();
	void 						render();
	void 						render3d(VkCommandBuffer cmd, RenderObject& object, Mesh* lastMesh, Material* lastMaterial);
	void 						render2d(VkCommandBuffer cmd, RenderObject& object, Mesh* lastMesh, Material* lastMaterial);
	void 						renderscene(VkCommandBuffer cmd);
	
	void  						ui();
	void 						debuglight();
	void 						debuganim();
	void 						debug2d(bool* active);
	void 						debug3d(bool* active);
	void 						debugmouse();
	void  						fpsoverlay();

	void 						initscene();
	void 						initvulkan();
	void 						initimgui();

	void 						initresources();
	void 						initmeshes();
	void 						reloadresources();
};

