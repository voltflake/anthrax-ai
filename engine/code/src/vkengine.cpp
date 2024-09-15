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

	vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) vkGetInstanceProcAddr(Builder.getinstance(), "vkCmdBeginRenderingKHR");
	vkCmdEndRenderingKHR   = (PFN_vkCmdEndRenderingKHR) vkGetInstanceProcAddr(Builder.getinstance(), "vkCmdEndRenderingKHR");

	initscene();
	initimgui();

}

void Engine::initvulkan() {

	Builder.buildinstance();

#if defined(AAI_WINDOWS)
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(WindowExtend, hwnd);
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
//	Builder.buildrenderpass();

//	Builder.builframebuffers();
	
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

float rand_float(float a, float b)
{
	float r = (float)rand() / (float)RAND_MAX;
	float diff = b - a;
	return a + (r * diff);
}

void Engine::initengine(LevelManager &levels) {
	EditorCamera.setposition({ 0.f, 0.f, 3.0f});
	EditorCamera.setdirections();

	DirectionLight.init(glm::vec3(-2.2f, 1.0f, 2.0f), glm::vec3(0.63f, 0.82f, 0.48f), LIGHT_DIRECTIONAL);
	printf("------------\n");
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		glm::vec3 randpos(rand_float(-10.0f, 10.0f), rand_float(-10.0f, 10.0f), rand_float(-10.0f, 10.0f));
		printf("point light [%d] x: %f, y: %f, z: %f \n", i, randpos.x,  randpos.y,  randpos.z);
		PointLights[i].init(randpos, glm::vec3(0.63f, 0.82f, 0.48f), LIGHT_POINT);
	}

	Level.getbackground().setposition({0, 0});
	Level.getbackground().setpath("placeholder.jpg");
	Level.gettrigger().reserve(10);
	Level.getobject().reserve(10);

	Resources.add(TYPE_BACKGROUND, 
	Data(Level.getbackground().getpath(), {0, 0, 0}, false, false));
}

void Engine::initscene() {

	Builder.procscene(Resources.get(), animator);
}


void Engine::reloadresources() {
	vkDeviceWaitIdle(Builder.getdevice()); // test it

	EditorCamera.setposition({ 0.f, 0.f, 3.0f});
	EditorCamera.setdirections();

	Builder.clearimages();
	Builder.cleartextureset();
	Builder.clearmeshes();

	Resources.clear();
	Debug.clear();

	Builder.renderqueue.clear();

	if (Level.test3d) {
		Builder.initmeshbuilder();
		initmeshes();

		initresources();

		Builder.inittexture(Resources.get());
		Builder.loadimages();
	}
	else {
		initresources();

		Builder.inittexture(Resources.get());
		Builder.loadimages();

		Builder.initmeshbuilder();
		initmeshes();
	}
	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.updatemodeldescriptors();

	initscene();

	for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->animation) {
   			Debug.ImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getobject()[i]->getpath())->sampler, Builder.gettexture(Level.getobject()[i]->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getobject()[i]->getpath()});
		}
	}
	if (Level.getplayer()->animation) {
   		Debug.ImGuiAnim.push_back({1, ImGui_ImplVulkan_AddTexture(Builder.gettexture(Level.getplayer()->getpath())->sampler, Builder.gettexture(Level.getplayer()->getpath())->imageview, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), Level.getplayer()->getpath() });
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
//	Builder.clearframebuffers();
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
