#include "../includes/vkengine.h"

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
    ImGui_ImplX11_Init(connection, &window);

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

	Builder.renderer.immediatesubmit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
	});

	ImGui_ImplVulkan_DestroyFontUploadObjects();

	Builder.deletorhandler.pushfunction([=]() {
		vkDestroyDescriptorPool(Builder.getdevice(), imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	});
}

void Engine::checkuistate() {

	if (state & NEW_LEVEL) {
		Level.newlevel();
	}
	if (state & LOAD_LEVEL && Level.loaded  == false) {
		Level.loadlevel();
		if (Level.initres && !Level.check) {
        	state |= NEW_LEVEL;
		}
	}
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
	
	fpsoverlay();

	active = true;
	if (state & PLAY_GAME) {
		active = false ;
	}
	if (!active) {
		return;
	}
   	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 40), 0);
	ImGui::SetNextWindowSize(ImVec2(500, 85), ImGuiCond_FirstUseEver);

	ImGui::Begin("Engine ;p", &active, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    
	ImGui::Columns(2, "enginebuttons", false);
	if (ImGui::Button("New Level")) {
        state |= NEW_LEVEL;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Load Level")) {
        state |= LOAD_LEVEL;
		Level.loaded = false;
	}
	ImGui::NextColumn();
	ImGui::Separator();

	ImGui::Columns(2, "enginebuttons2", false);
	if (ImGui::Button("Play")) {
		active = false;
		state |= PLAY_GAME;
		state ^= ENGINE_EDITOR;
	}
	ImGui::NextColumn();
	if (ImGui::Button("Close")) {
       	active = false;
	}
	ImGui::NextColumn();

	ImGui::Columns(1);
    ImGui::Checkbox("Free Move", &freemove);

	checkuistate();

    ImGui::End();
}

void Engine::fpsoverlay() {
    static bool active = true;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowPos( ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    flags |= ImGuiWindowFlags_NoMove;
  	if (ImGui::Begin("FPS overlay", &active, flags)) {
        ImGui::Text("FPS: %f", fps);
        ImGui::Separator();
	}
    ImGui::End();
}
