#include "anthraxAI/vkengine.h"

void Engine::initimgui() {
	
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_ASSERT(vkCreateDescriptorPool(Builder.getdevice(), &pool_info, nullptr, &imguiPool), "failed to creat imgui descriptor set!");

	ImGui::CreateContext();
#if defined(AAI_LINUX)
    ImGui_ImplX11_Init(connection, &window);
#elif defined(AAI_WINDOWS)
	ImGui_ImplWin32_Init(hwnd);
#endif

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

	ImGui::StyleColorsDark();
    io.Fonts->AddFontDefault();
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Builder.getinstance();
	init_info.PhysicalDevice = Builder.getphysicaldevice();
	init_info.Device = Builder.getdevice();
	init_info.Queue = Builder.getqueue().graphicsqueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, Builder.getrenderpass());

	Builder.renderer.submit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
	});

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	Builder.deletorhandler.pushfunction([=]() {
		vkDestroyDescriptorPool(Builder.getdevice(), imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	});

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0;
	style.WindowRounding = 3;
	style.GrabRounding = 1;
	style.GrabMinSize = 20;
	style.FrameRounding = 3;

	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
	EditorStyle = style;


	TextDisplayStyle = EditorStyle;

	TextDisplayStyle.Alpha = 1.0f;
	TextDisplayStyle.FrameRounding = 3.0f;
	TextDisplayStyle.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	TextDisplayStyle.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	TextDisplayStyle.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	TextDisplayStyle.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	TextDisplayStyle.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	TextDisplayStyle.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	TextDisplayStyle.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	TextDisplayStyle.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	TextDisplayStyle.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	TextDisplayStyle.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	TextDisplayStyle.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	TextDisplayStyle.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	TextDisplayStyle.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	TextDisplayStyle.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	TextDisplayStyle.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	TextDisplayStyle.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}

void Engine::animator2d() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 500, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(500, 800), ImGuiCond_FirstUseEver);
	
	ImGui::Begin("Vulkan Texture Test");
	for (int i = 0; i < DebugImGuiAnim.size(); i++) {
		ImGui::TextUnformatted(DebugImGuiAnim[i].path.c_str());
		ImGui::SliderInt(std::string("scale " + std::to_string(i)).c_str(), &DebugImGuiAnim[i].scale, 1, 8);
		ImGui::Image((ImTextureID)DebugImGuiAnim[i].desc, ImVec2(Builder.gettexture(DebugImGuiAnim[i].path)->w / DebugImGuiAnim[i].scale, Builder.gettexture(DebugImGuiAnim[i].path)->h / DebugImGuiAnim[i].scale));
		ImGui::Separator();
	}
	ImGui::End();
}

void Engine::checkuistate() {
	bool active = true;
	if (state & MODE_2D) {
		debug2d(&active);
	}
	if (state & MODE_3D) {
		debug3d(&active);
	}
	if (state & NEW_LEVEL) {
		Level.newlevel();
	}

}

void Engine::debug2d(bool* active) {
	ImGui::Columns(4, "enginebuttons", false);
	if (ImGui::Button("New Level")) {
        state |= NEW_LEVEL;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Load Level")) {
        state |= LOAD_LEVEL;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Play")) {
		*active = false;
		state |= PLAY_GAME;
		state ^= ENGINE_EDITOR;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Close")) {
       	state |= EXIT;
	}
	ImGui::NextColumn();
	ImGui::Columns(1);
	ImGui::Separator();
	if (state & LOAD_LEVEL && Level.loaded  == false) {
		Level.loadlevel();
		if (Level.initres && !Level.check) {
        	state |= NEW_LEVEL;
		}
	}
	ImGui::Separator();
    ImGui::Checkbox("Free Move", &freemove);

	static bool anim = false;
	ImGui::Checkbox("Animator 2d", &anim);
	if (anim) {
		animator2d();
	}
}

void Engine::debug3d(bool* active) {
	ImGui::Columns(4, "engine3dbuttons", false);
	if (ImGui::Button("New Level")) {
        state |= NEW_LEVEL;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Load Level")) {
        state |= LOAD_LEVEL;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Play")) {
		*active = false;
		state |= PLAY_GAME;
		state ^= ENGINE_EDITOR;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Close")) {
       	state |= EXIT;
	}
	ImGui::NextColumn();
	ImGui::Columns(1);
	ImGui::Separator();
	if (state & LOAD_LEVEL && Level.loaded  == false) {
		Level.loadlevel();
		if (Level.initres && !Level.check) {
        	state |= NEW_LEVEL;
		}
	}
	ImGui::Separator();
	static bool light = false;
	ImGui::Checkbox("Lighting", &light);
	if (light) {
		debuglight();
	}

	ImGui::Separator();
	static bool anim = false;
	ImGui::Checkbox("Animation", &anim);
	if (anim) {
		debuganim();
	}
	ImGui::Separator();
}

void Engine::debuglight()
{
	ImGuiStyle& stylem = ImGui::GetStyle();
	stylem = EditorStyle;
   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 500, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	static bool active = true;
	ImGui::Begin("Lighting", &active, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings );
    
	ImGui::TextUnformatted("light position");
	ImGui::SliderFloat("x", &camdata.lightpos.x, -5.0f, 5.0f, "%.2f"); 
	ImGui::SliderFloat("y", &camdata.lightpos.y, -5.0f, 5.0f, "%.2f"); 
	ImGui::SliderFloat("z", &camdata.lightpos.z, -5.0f, 5.0f, "%.2f");
	ImGui::Separator();
	
	ImGui::TextUnformatted("light color");
	ImGui::SliderFloat("r", &camdata.lightcolor.x, -5.0f, 5.0f, "%.2f");
	ImGui::SliderFloat("g", &camdata.lightcolor.y, -5.0f, 5.0f, "%.2f");
	ImGui::SliderFloat("b", &camdata.lightcolor.z, -5.0f, 5.0f, "%.2f");
	ImGui::Separator();
	ImGui::SliderFloat("ambient", &camdata.ambient, -5.0f, 5.0f, "%.2f");
	ImGui::SliderFloat("specular", &camdata.specular, -5.0f, 5.0f, "%.2f");
	ImGui::End();
}

void Engine::debuganim()
{
	ImGuiStyle& stylem = ImGui::GetStyle();
	stylem = EditorStyle;
   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 500, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	static bool active = true;
	ImGui::Begin("Animation", &active, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings );
    
	ImGui::SliderFloat("anim speed", &animspeed, 0.0f, 2.0f, "%.2f");
	ImGui::Checkbox("debug bones", &debugbones);
	ImGui::TextUnformatted(std::string("Bone id: " + std::to_string(debugboneID)).c_str());

	for (auto& animation : animator.animations) {
		ImGui::TextUnformatted(std::string("Object id: " + std::to_string(animation.first)).c_str());
		const char* preview = animation.second.selectedpath.c_str();
		if (ImGui::BeginCombo(std::string("##c" +  std::to_string(animation.first) + "animation").c_str(), preview)) {
			for (int i = 0; i < animation.second.animpaths.size(); ++i) {
				const bool isSelected = (animation.second.pathindex == i);
				if (ImGui::Selectable(animation.second.animpaths[i].c_str(), isSelected)) {
					animation.second.pathindex = i;
					animator.reload(animation.first);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
    		ImGui::EndCombo();
		}
	}
	ImGui::End();
}

void Engine::debugmouse()
{
	static bool active = true;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 500, 0), 0);
    flags |= ImGuiWindowFlags_NoMove;
  	ImGui::Begin("Mouse state", &active, flags);
	std::string state = "IDLE";
	if (mousestate == MOUSE_IDLE) {
		state = "MOUSE_IDLE";
	}
	if (mousestate == MOUSE_MOVE) {
		state = "MOUSE_MOVE";		
	}
	if (mousestate == MOUSE_PRESSED) {
		state = "MOUSE_PRESSED";
	}
	if (mousestate == MOUSE_RELEASED) {
		state = "MOUSE_RELEASED";	
	}
	if (mousestate == MOUSE_SELECTED) {
		state = "MOUSE_SELECTED";		
	}
	ImGui::Text("mouse state: %s", state.c_str());
	ImGui::Separator();
    ImGui::End();
}

void Engine::ui() {
        
	if (checkupdate) {
		checkupdate = false;
		checkimg++;
		loadmylevel();
	}

    if (Level.check2 || Level.check) {
    	return;
    }
    bool active = true;

    // ImGui::ShowDemoWindow();

	if (state & PLAY_GAME) {
		active = false ;
	}
	if (!active) {
		return;
	}
	ImGuiStyle& stylem = ImGui::GetStyle();
	stylem = EditorStyle;
   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(400, viewport->Size.y - 40), ImGuiCond_FirstUseEver);

	ImGui::Begin("Engine ;p", &active, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    
	ImGuiStyle& style = EditorStyle;
	float alpha = style.Colors[ImGuiCol_WindowBg].w;
    ImGui::SliderFloat("Editor Alpha", &alpha, 0.0, 1.0, "%.1f");
	if (style.Colors[ImGuiCol_WindowBg].w != alpha) {
		style.Colors[ImGuiCol_WindowBg].w = alpha;
	}
	ImGui::Separator();
	ImGui::Columns(2, "modes");
	if (ImGui::Button("2d")) {
		state &= ~MODE_3D;
		state |= MODE_2D;
	}
	ImGui::NextColumn();
	if (ImGui::Button("3d")) {
		state &= ~MODE_2D;
		state |= MODE_3D;
	}
	ImGui::Separator();
	ImGui::NextColumn();
	ImGui::Columns(1);
	checkuistate();

    ImGui::End();


	debugmouse();
}

void Engine::fpsoverlay() {
    static bool active = true;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowPos( ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    flags |= ImGuiWindowFlags_NoMove;
  	ImGui::Begin("FPS overlay", &active, flags);
        ImGui::Text("FPS: %f", fps);
        ImGui::Separator();
    ImGui::End();
}
