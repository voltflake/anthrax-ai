#include "../includes/vkengine.h"

void Engine::init() {
	initengine(Levels);
	initvulkan();
	initscene();
	initimgui();
}

void Engine::initvulkan() {

	Builder.buildinstance();

#ifdef OS_WINDOWS
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(hwnd);
#endif
#ifdef OS_LINUX
	Builder.buildlinuxsurface(connection, window);
	Builder.initdevicebuilder(WindowExtend);
#endif
	Builder.buildphysicaldevice();

	Builder.buildlogicaldevice();

	Builder.buildswapchain();
	Builder.buildimagesview();
	
	Builder.initrenderbuilder();
	Builder.buildcommandpool();

	Builder.buildrenderpass();
	Builder.builframebuffers();
	
	Builder.startsync();

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initpipelinebuilder();
	Builder.buildpipeline(Levels.check);

	Builder.initmeshbuilder();
	Builder.loadmeshes(resources);

}

void Engine::initengine(LevelManager &levels) {

	levels.level.background.x = 0;
	levels.level.background.y = 0;

	resources["placeholder.jpg"] = {levels.level.background.x, levels.level.background.y};
}

void Engine::initscene() {

	int i = 0;
	for (auto& list : resources) {
		if (list.first == "") {
            continue;
        }
		Builder.descriptors.updatesamplerdescriptors(list.first);

		RenderObject tri;
		tri.mesh = Builder.getmesh(list.first);
		tri.material = Builder.getmaterial("defaultmesh");

		tri.textureset = &Builder.getsamplerset()[i];
		Builder.pushrenderobject(tri);

		i++;
	}
}

void Engine::initresources()
{
	if (Levels.level.trigger.path != "") {
		resources[Levels.level.trigger.path] = {Levels.level.trigger.x, Levels.level.trigger.y};
	}
	if (Levels.level.npc.path != "") {
		resources[Levels.level.npc.path] = {Levels.level.npc.x, Levels.level.npc.y};	
	}
	if (Levels.level.object.path != "") {
		resources[Levels.level.object.path] = {Levels.level.object.x, Levels.level.object.y};
	}
		if (Levels.level.player.path != "") {
		resources[Levels.level.player.path] = {Levels.level.player.x, Levels.level.player.y}; // player has to be always second -- stupid
	}
	if (Levels.level.background.path != "") {
		resources[Levels.level.background.path] = {Levels.level.background.x, Levels.level.background.y}; // background for some reason should be always top, looks kinda broken
	}
}

void Engine::reloadresources() {
	vkDeviceWaitIdle(Builder.getdevice()); // test it

	Builder.clearimages();
	Builder.cleartextureset();
	Builder.clearmeshes();

	resources.clear();

	Builder.renderqueue.clear();

	initresources();

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initmeshbuilder();
	Builder.loadmeshes(resources);

	initscene();
}

void Engine::cleanup() {
	vkDeviceWaitIdle(Builder.getdevice());

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearmeshes();
	resources.clear();
	Builder.renderqueue.clear();

	Builder.clearpipeline();
	Builder.cleanswapchain();
	Builder.clearframebuffers();
	Builder.cleanall();

    ImGui_ImplX11_Shutdown();
    ImGui::DestroyContext();
}
