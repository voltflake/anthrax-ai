#include "anthraxAI/vkengine.h"

void Engine::run() {
#if defined(AAI_LINUX)
	runlinux();
#elif defined(AAI_WINDOWS)
	runwindows();
#endif
}

void Engine::init() {
#if defined(AAI_LINUX)
	linuxinitwindow();
#elif defined(AAI_WINDOWS)
	wininitwindow();
#endif

	initengine(Level);
	initvulkan();
	initscene();
	initimgui();

}

void Engine::initvulkan() {

	Builder.buildinstance();

#if defined(AAI_WINDOWS)
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(hwnd);
#endif
#ifdef AAI_LINUX
	Builder.buildlinuxsurface(connection, window);
	Builder.initdevicebuilder(WindowExtend);
#endif
	Builder.buildphysicaldevice();

	Builder.buildlogicaldevice();

	Builder.buildswapchain();
	Builder.buildimagesview();
	
	Builder.initrenderbuilder();
	Builder.buildcommandpool();

	Builder.builddepthbuffer();
	Builder.buildmainimage();
	Builder.buildrenderpass();

	Builder.builframebuffers();
	
	Builder.startsync();

	Builder.inittexture(Resources.get());
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initpipelinebuilder();
	Builder.buildpipeline(Level.check);

	Builder.initmeshbuilder();
	Builder.loadmeshes();
}

void Engine::initengine(LevelManager &levels) {
	EditorCamera.setposition({ 0.f, 0.f, 3.0f});
	EditorCamera.setdirections();

	Level.getbackground().setposition({0, 0});
	Level.getbackground().setpath("placeholder.jpg");
	Level.gettrigger().reserve(10);
	Level.getobject().reserve(10);

	Resources.add(TYPE_BACKGROUND, 
	Data(Level.getbackground().getpath(), {0, 0, 0}, false, false));
}

void Engine::initscene() {

	Builder.procscene(Resources.get(), animator);
	// int i = 0;
	// int objcounter = 1;

	// ResourcesMap resmap = Resources.get();
	// for (auto& list : resmap) {
	// 	if (list.second.texturepath == "") {
    //         continue;
    //     }

	// 	RenderObject tri;

	// 	if (list.first >= TYPE_MODEL) {
	// 		tri.ID = objcounter;
	// 		tri.type = TYPE_MODEL;
	// 		objcounter++;
	// 		if (list.first >= TYPE_GIZMO) {
	// 			tri.type = TYPE_GIZMO;
	// 			tri.ID = list.first;
	// 		}
	// 		Builder.descriptors.updatesamplerdescriptors(list.second.texturepath);
	// 		if (animator.hasanimation(list.first)) {
	// 			tri.material = Builder.getmaterial("animated");
	// 			tri.animated = true;
	// 			tri.ID = list.first;
	// 		}
	// 		else {
	// 		tri.material = Builder.getmaterial("monkey");

	// 		}
	// 		tri.model = Builder.getmodel(list.first);
	// 	}
	// 	else {
	// 		Builder.descriptors.updatesamplerdescriptors(list.second.texturepath);
	// 		tri.material = Builder.getmaterial("defaultmesh");
	// 		tri.mesh = Builder.getmesh(list.first);

	// 	}

	// 	tri.pos = list.second.pos;
	// 	tri.textureset = &Builder.getsamplerset()[i];
	// 	tri.debugcollision = list.second.debugcollision;
	// 	Builder.pushrenderobject(tri);
	// 	i++;
	//}

	// RenderObject debug;
	// debug.material = Builder.getmaterial("debug");
	// debug.debug = true;
	// Builder.pushrenderobject(debug);
}


void Engine::reloadresources() {
	vkDeviceWaitIdle(Builder.getdevice()); // test it

	Builder.clearimages();
	Builder.cleartextureset();
	Builder.clearmeshes();

	Resources.clear();

	Builder.renderqueue.clear();

	initresources();

	Builder.inittexture(Resources.get());
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initmeshbuilder();

	initmeshes();

	initscene();

	for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->animation) {
   			DebugImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getobject()[i]->getpath())->sampler, Builder.gettexture(Level.getobject()[i]->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getobject()[i]->getpath()});
		}
	}
	if (Level.getplayer()->animation) {
   		DebugImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getplayer()->getpath())->sampler, Builder.gettexture(Level.getplayer()->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getplayer()->getpath() });
	}
}

void Engine::cleanup() {
	vkDeviceWaitIdle(Builder.getdevice());

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearattachments();
	Builder.clearmeshes();
	
	Resources.clear();
	Builder.renderqueue.clear();

	Builder.clearpipeline();
	Builder.cleanswapchain();
	Builder.clearframebuffers();
	Builder.cleanall();
	for (int i = 0; i < Level.getobject().size(); i++) {
		delete Level.getobject()[i];
	}

#if defined(AAI_LINUX)
    ImGui_ImplX11_Shutdown();
#elif defined(AAI_WINDOWS)
	ImGui_ImplWin32_Shutdown();
#endif
    ImGui::DestroyContext();
}
