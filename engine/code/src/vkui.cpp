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
	init_info.Subpass = 1;

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
	Debug.EditorStyle = style;


	Debug.TextDisplayStyle = Debug.EditorStyle;

	Debug.TextDisplayStyle.Alpha = 1.0f;
	Debug.TextDisplayStyle.FrameRounding = 3.0f;
	Debug.TextDisplayStyle.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	Debug.TextDisplayStyle.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
}

void Engine::animator2d() {

   	Editor.beginwindow( { -500, 40 }, { 500, 800 }, ImGuiCond_FirstUseEver, "Vulkan Texture Test");

	for (int i = 0; i < Debug.ImGuiAnim.size(); i++) {
		ImGui::TextUnformatted(Debug.ImGuiAnim[i].path.c_str());
		ImGui::SliderInt(std::string("scale " + std::to_string(i)).c_str(), &Debug.ImGuiAnim[i].scale, 1, 8);
		ImGui::Image((ImTextureID)Debug.ImGuiAnim[i].desc, ImVec2(Builder.gettexture(Debug.ImGuiAnim[i].path)->w / Debug.ImGuiAnim[i].scale, Builder.gettexture(Debug.ImGuiAnim[i].path)->h / Debug.ImGuiAnim[i].scale));
		ImGui::Separator();
	}
	
	Editor.endwindow();
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
    if (Level.test3d) return;

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
    ImGui::Checkbox("Free Move", &Debug.freemove);

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
    Editor.beginwindow( { -500, 40 }, { 500, 400 }, ImGuiWindowFlags_NoSavedSettings, "Lighting");

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

	Editor.endwindow();
}

void Engine::debuganim()
{
	ImGuiStyle& stylem = ImGui::GetStyle();
	stylem = Debug.EditorStyle;

   	Editor.beginwindow( { -500, 40 }, { 500, 400 }, ImGuiWindowFlags_NoSavedSettings, "Animation");

	ImGui::SliderFloat("anim speed", &Debug.animspeed, 0.0f, 2.0f, "%.2f");
	ImGui::Checkbox("debug bones", &Debug.bones);
	ImGui::TextUnformatted(std::string("Bone id: " + std::to_string(Debug.boneID)).c_str());

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

	Editor.endwindow();
}

void Engine::debugmouse()
{

   	Editor.beginwindow( { -500, 0 }, { 0, 0 },
						ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav, 
						"Mouse state");

	std::string state = "IDLE";
	if (Mouse.state == MOUSE_IDLE) {
		state = "MOUSE_IDLE";
	}
	if (Mouse.state == MOUSE_MOVE) {
		state = "MOUSE_MOVE";		
	}
	if (Mouse.state == MOUSE_PRESSED) {
		state = "MOUSE_PRESSED";
	}
	if (Mouse.state == MOUSE_RELEASED) {
		state = "MOUSE_RELEASED";	
	}
	if (Mouse.state == MOUSE_SELECTED) {
		state = "MOUSE_SELECTED";		
	}
	ImGui::Text("mouse state: %s", state.c_str());
	ImGui::Separator();

	Editor.endwindow();
}

void Engine::ui() {

    bool active = true;

   // ImGui::ShowDemoWindow();

	if (state & PLAY_GAME) {
		active = false ;
	}
	if (!active) {
		return;
	}

   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(400, viewport->Size.y - 40), ImGuiCond_FirstUseEver);

	ImGui::Begin("Engine ;p", &active, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    
	ImGuiStyle& style = ImGui::GetStyle();
	//stylem = Debug.EditorStyle;
	style = Debug.EditorStyle;
	float alpha = style.Colors[ImGuiCol_WindowBg].w;

    ImGui::SliderFloat("Editor Alpha", &alpha, 0.0, 1.0, "%.1f");
	if (style.Colors[ImGuiCol_WindowBg].w != alpha) {
		Debug.EditorStyle.Colors[ImGuiCol_WindowBg].w = alpha;
	}
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("WARNING ! Issues remains !\nPress 2D or 3D button (changes debug functionality)\nType '0' in the input field to load 2D level \nType '1' in the inputfield to load 3D level\nUse WASD to move camera, for rotation use LMB\nPress ESC to switch between 'Editor' and 'Play' mode");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
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
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    flags |= ImGuiWindowFlags_NoMove;

    static bool active = true;

	ImGui::SetNextWindowPos( ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
  	ImGui::Begin("FPS overlay", &active, flags);
        ImGui::Text("FPS: %f", Debug.fps);
        ImGui::Separator();
   ImGui::End();
}
