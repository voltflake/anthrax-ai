#include "anthraxAI/core/imguihelper.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/utils/debug.h"
#include "imgui.h"
#include <algorithm>
#include <iterator>
#include <string>

void Core::ImGuiHelper::UpdateFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplX11_NewFrame();
	ImGui::NewFrame();
}

Core::ImGuiHelper::~ImGuiHelper()
{
#if defined(AAI_LINUX)
    ImGui_ImplX11_Shutdown();
#elif defined(AAI_WINDOWS)
	ImGui_ImplWin32_Shutdown();
#endif
    ImGui::DestroyContext();
}

void Core::ImGuiHelper::Init()
{
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
	VK_ASSERT(vkCreateDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), &pool_info, nullptr, &imguiPool), "failed to creat imgui descriptor set!");

	ImGui::CreateContext();
#if defined(AAI_LINUX)
    ImGui_ImplX11_Init(Core::WindowManager::GetInstance()->GetConnection(), Core::WindowManager::GetInstance()->GetWindow());
#elif defined(AAI_WINDOWS)
	ImGui_ImplWin32_Init(hwnd);
#endif

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

	ImGui::StyleColorsDark();
    io.Fonts->AddFontDefault();
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Gfx::Vulkan::GetInstance()->GetVkInstance();
	init_info.PhysicalDevice =  Gfx::Device::GetInstance()->GetPhysicalDevice();
	init_info.Device = Gfx::Device::GetInstance()->GetDevice();
	init_info.Queue = Gfx::Device::GetInstance()->GetQueue(Gfx::GRAPHICS_QUEUE);
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, nullptr);

    Gfx::Renderer::GetInstance()->Submit([&](VkCommandBuffer cmd) {
        ImGui_ImplVulkan_CreateFontsTexture(cmd);
    });

	ImGui_ImplVulkan_DestroyFontUploadObjects();

    Core::Deletor::GetInstance()->Push([=, this]() {
        vkDestroyDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), imguiPool, nullptr);
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
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.60f, 0.61f, 0.80f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.40f, 0.41f, 0.40f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);

	EditorStyle = style;
    InitUIElements();
}

void Core::ImGuiHelper::InitUIElements()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 pos = viewport->Pos;
    {
        EditorWindow = "Engine ;p";
        Add(EditorWindow, UI::Window(EditorWindow, { 400.0f, Core::WindowManager::GetInstance()->GetScreenResolution().y - 40.0f }, { pos.x, pos.y + 40.0f }, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings ));
        
        std::string tablabel = "Editor";
        UI::Element tab(UI::TAB, tablabel);

        std::vector<const char*> scenes;
        scenes.reserve(Core::Scene::GetInstance()->GetSceneNames().size());
        for (const std::string& it : Core::Scene::GetInstance()->GetSceneNames()) {
            scenes.emplace_back(it.c_str());
        }

        Add(tab, UI::Element(UI::COMBO, "Scenes", scenes, [](std::string tag) -> void { Core::Scene::GetInstance()->SetCurrentScene(tag); }));
        
        Add(tab, UI::Element(UI::SEPARATOR, "tabseparator"));
    
        Add(EditorWindow, UI::Element(UI::BUTTON, "Global Button")); 
    }
    
    UI::Element debugtab(UI::TAB, "Debug");
    Add(debugtab, UI::Element(UI::TEXT, "This is debug tab"));
    Add(debugtab, UI::Element(UI::SEPARATOR, "sep"));
    Add(debugtab, UI::Element(UI::FLOAT, "fps", []() -> float { return Utils::Debug::GetInstance()->FPS; }));
    Add(debugtab, UI::Element(UI::SEPARATOR, "sep"));
    Add(debugtab, UI::Element(UI::CHECKBOX, "3d grid", nullptr, [](bool visible) -> void {  Utils::Debug::GetInstance()->Grid = visible; }));
}

void Core::ImGuiHelper::Combo(UI::Element element) const
{
    std::vector<const char*> items = element.GetComboList() ; 
    auto it = std::find(items.begin(), items.end(), "intro");
    static int ind = std::distance(items.begin(), it) - 1;

    const char* currvalue = items[ind];  
    if (ImGui::BeginCombo(element.GetLabel().c_str(), currvalue, 0)) {
        for (int n = 0; n < items.size(); n++) {
            const bool is_selected = (ind == n);
            if (ImGui::Selectable(items[n], is_selected)) {
                ind = n;
                element.Definition(items[ind]);
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void Core::ImGuiHelper::ProcessUI(const UI::Element& element)
{
    switch (element.GetType()) {
        case UI::COMBO: {
            Combo(element);
            break;
        }
        case UI::FLOAT: {
            ImGui::Text((element.GetLabel() + ": %f").c_str(), element.DefinitionFloat());
            break;
        }
        case UI::BUTTON: {    
            if (ImGui::Button(element.GetLabel().c_str())) {
                if (element.Definition) {
                    element.Definition(element.GetLabel());
                }
            }
            break;
        }
        case UI::CHECKBOX: {
            static bool check = true; 
            ImGui::Checkbox(element.GetLabel().c_str(), &check);
            element.DefinitionBool(check);
            break;
        }
        case UI::TEXT:
            ImGui::TextUnformatted(element.GetLabel().c_str());
            break;
        case UI::SEPARATOR:
            ImGui::Separator();
        default:
            break;
    }
}

void Core::ImGuiHelper::Render()
{
    bool active = true;

    std::vector<UI::Window>& windows = UIWindows[EditorWindow];
    std::vector<UI::Element>& windowelements = UIElements[EditorWindow];

    for (UI::Window& window : windows) {
        ImGui::SetNextWindowPos(ImVec2(window.GetPosX(), window.GetPosY()), 0);
        ImGui::SetNextWindowSize(ImVec2(window.GetSizeX(), window.GetSizeY()), ImGuiCond_FirstUseEver);

        ImGui::Begin(window.GetName().c_str(), &active, window.GetFlags());
        for (auto& it : UITabs) {
            std::vector<UI::Element>& tabsui = it.second;
            UI::Element tab = it.first;

            if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_None)){
                if (ImGui::BeginTabItem(tab.GetLabel().c_str())) {
                    for (UI::Element& element : tabsui) {
                        ProcessUI(element);
                    }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        
        for (UI::Element& element : windowelements) {
            ProcessUI(element);
        }

        ImGui::End();
    }
}


