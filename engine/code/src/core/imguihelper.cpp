#include "anthraxAI/core/imguihelper.h"
#include "anthraxAI/core/audio.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/utils/debug.h"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

void Core::ImGuiHelper::UpdateFrame()
{
	ImGui_ImplVulkan_NewFrame();
#ifdef AAI_LINUX
	ImGui_ImplX11_NewFrame();
#else
	ImGui_ImplWin32_NewFrame();
#endif
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
	ImGui_ImplWin32_Init(Core::WindowManager::GetInstance()->GetWinWindow());
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

    Core::Deletor::GetInstance()->Push(Core::Deletor::Type::NONE, [=, this]() {
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

    Initialized = true;

    Gfx::Renderer::GetInstance()->CreateImGuiDescSet();
}

void UI::Element::GetArg(const std::vector<std::string>& vec)
{
    if (!ComboList.empty()) {
        ComboList.clear();
    }
    ComboList.reserve(vec.size() + 1);
    if (AddEmpty) {
        ComboList.emplace_back("none");
    }
    for (const std::string& s : vec) {
        ComboList.emplace_back(s);
    }
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
        Add(tab, UI::Element(UI::COMBO, "Scenes", false, Core::Scene::GetInstance()->GetSceneNames(), [](std::string tag) -> void { Core::Scene::GetInstance()->SetCurrentScene(tag); }, true));
        Add(tab, UI::Element(UI::SEPARATOR, "tabseparator"));
        Add(EditorWindow, UI::Element(UI::BUTTON, "Update Shaders", false, []() -> float { Gfx::Vulkan::GetInstance()->ReloadShaders(); return 0.0f; }));
        Add(EditorWindow, UI::Element(UI::CHECKBOX, "Keep Editor", false, nullptr,[](bool show) -> void {  Core::Scene::GetInstance()->KeepEditor(show); }));
    }

    {
        UI::Element scenetab(UI::TAB, "Scene");
        Add(scenetab, UI::Element(UI::TEXT, "Name:", false, []() -> std::string { return Core::Scene::GetInstance()->GetCurrentScene(); } ));
        Add(scenetab, UI::Element(UI::SEPARATOR, "sep"));
    }

    {
        UI::Element rendertab(UI::TAB, "Rendering");
        Add(rendertab, UI::Element(UI::COMBO, "Render Targets", false, Gfx::Renderer::GetInstance()->GetRTList(), [](std::string tag) -> void { ImGuiHelper::GetInstance()->SetDebugRT(tag); }, true));
        Add(rendertab, UI::Element(UI::DEBUG_IMAGE, "image", false));
        Add(rendertab, UI::Element(UI::SEPARATOR, "sep"));
    }

    {
        UI::Element audiotab(UI::TAB, "Audio");
        Add(audiotab, UI::Element(UI::COMBO, "Sounds", false, Core::Audio::GetInstance()->GetAudioNames(), [](std::string tag) -> void { Core::Audio::GetInstance()->Load(tag); }, true));
        Add(audiotab, UI::Element(UI::SEPARATOR, "sep"));
        Add(audiotab, UI::Element(UI::TEXT, "Current Sound:", false, []() -> std::string { return Core::Audio::GetInstance()->GetCurrentSound(); } ));
        Add(audiotab, UI::Element(UI::SEPARATOR, "sep"));
        Add(audiotab, UI::Element(UI::CHECKBOX, "play", false, nullptr, [](bool visible) -> void {  Core::Audio::GetInstance()->SetState(visible); }));
        Add(audiotab, UI::Element(UI::SLIDER, "volume", false, [](float volume) -> float { Core::Audio::GetInstance()->SetVolume(volume); return 0.0f; }));
    }

    {
        UI::Element debugtab(UI::TAB, "Debug");
        Add(debugtab, UI::Element(UI::TEXT, "This is debug tab"));
        Add(debugtab, UI::Element(UI::SEPARATOR, "sep"));
        Add(debugtab, UI::Element(UI::FLOAT, "fps", false, []() -> float { return Utils::Debug::GetInstance()->FPS; }));
        Add(debugtab, UI::Element(UI::SEPARATOR, "sep"));
        Add(debugtab, UI::Element(UI::CHECKBOX, "3d grid", false, nullptr, [](bool visible) -> void {  Utils::Debug::GetInstance()->Grid = visible; }));
        Add(debugtab, UI::Element(UI::CHECKBOX, "show bones weight", false, nullptr, [](bool show) -> void {  Utils::Debug::GetInstance()->Bones = show; }));
    }

}

Keeper::Objects* Core::ImGuiHelper::ParseObjectID(const std::string& id)
{
    Keeper::Type type;
    if (id.find("Camera:") != std::string::npos) {
        type = Keeper::Type::CAMERA;
    }
    else if (id.find("NPC:") != std::string::npos) {
        type = Keeper::Type::NPC;
    }
    else {
        type = Keeper::Type::SPRITE;
    }

    std::string trimstr = id;
    auto it = std::find_if(trimstr.begin(), trimstr.end(), [](char c) { return c == ':'; });

    std::string type_str(trimstr.begin(), it);

    trimstr.erase(trimstr.begin(), it + 1);
    Keeper::Objects* game_obj = nullptr;

    it = std::find_if(trimstr.begin(), trimstr.end(), [](char c) { return !(c >= '0' && c <= '9') && c != ' '; });
    if (it == trimstr.end()) {
        game_obj = const_cast<Keeper::Base*>(Core::Scene::GetInstance()->GetGameObjects())->GetNotConstObject(type, std::stoi(trimstr));
    }
    else {
        trimstr.erase(std::remove_if(trimstr.begin(), trimstr.end(), isspace));
        game_obj = const_cast<Keeper::Base*>(Core::Scene::GetInstance()->GetGameObjects())->GetNotConstObject(type, trimstr);
    }
    return game_obj;
}

void Core::ImGuiHelper::DisplayObjectInfo(const std::string& obj, const UI::Element& elem)
{
    if (!IsDisplayInReset) return;

    auto ui_it = std::remove_if(UITabs[elem].begin(), UITabs[elem].end(), [](const UI::Element& el) { return el.IsUIDynamic(); });
    const Keeper::Objects* game_obj = ParseObjectID(obj);

    UITabs[elem].erase(ui_it, UITabs[elem].end());
    Add(elem, UI::Element(UI::TEXT, "Type: ", true));
    Add(elem, UI::Element(UI::TEXT, "Position: ", true, [game_obj]() -> std::string { return game_obj->GetPosition().ToString(); } ));
    Add(elem, UI::Element(UI::TEXT, "Texture: " + game_obj->GetTextureName(), true));
    Add(elem, UI::Element(UI::TEXT, "Model: " + game_obj->GetModelName(), true));
    std::vector<std::string> s = { "Fragment: " + game_obj->GetFragmentName(), "Vertex: " + game_obj->GetVertexName() };
    Add(elem, UI::Element(UI::TREE, "Material: " + game_obj->GetMaterialName(), true, s ,[](std::string tag) -> void { return; }, false));

    Add(elem, UI::Element(UI::SEPARATOR, "sep", true));
    if (game_obj->HasAnimations()) {
        Add(elem, UI::Element(UI::TEXT, "Animations: ", true));
        Add(elem, UI::Element(UI::COMBO, "list", true, game_obj->GetAnimations(), [game_obj](std::string tag) -> void { Core::Scene::GetInstance()->ReloadAnimation(game_obj->GetID(), tag); }, false));
        Add(elem, UI::Element(UI::SEPARATOR, "sep", true));
    }

    Add(elem, UI::Element(UI::IMAGE, "Textures", true));
       Add(elem, UI::Element(UI::SEPARATOR, "sep", true));
    Add(elem, UI::Element(UI::BUTTON, "Update Object", true, [game_obj]() -> float { Core::Scene::GetInstance()->ExportObjectInfo(game_obj); return 0.0f; }));
    Add(elem, UI::Element(UI::SEPARATOR, "sep", true));
    IsDisplayInReset = false;
}

void Core::ImGuiHelper::UpdateObjectInfo()
{
    SelectedElement.clear();

    IsDisplayInReset = true;
    for (auto& it : UITabs) {
        if (it.first.GetLabel() == "Scene") {
            for (auto& elem : it.second) {
                elem.ClearComboList();
            }
            it.second.clear();

            Add(it.first, UI::Element(UI::TEXT, "Name:", false, []() -> std::string { return Core::Scene::GetInstance()->GetCurrentScene(); } ));
            Add(it.first, UI::Element(UI::SEPARATOR, "sep"));
            Add(it.first, UI::Element(UI::LISTBOX, "Objects", false, Core::Scene::GetInstance()->GetGameObjects()->GetObjectNames(), [this](std::string tag, const UI::Element& elem) -> void { DisplayObjectInfo(tag, elem); }, false));
            Add(it.first, UI::Element(UI::SEPARATOR, "sep"));
            break;
        }
    }
}

void Core::ImGuiHelper::Combo(UI::Element& element)
{
    if (ImGui::BeginCombo(element.GetLabel().c_str(), element.GetComboList()[element.ComboInd].c_str(), 0)) {
        size_t size = element.GetComboList().size();
        for (int n = 0; n < size; n++) {
            const bool is_selected = (element.ComboInd == n);
            if (ImGui::Selectable(element.GetComboList()[n].c_str(), is_selected)) {
                element.ComboInd = n;
                if (element.GetComboList()[n] != "none") {
                    element.Definition(element.GetComboList()[element.ComboInd]);
                }
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void Core::ImGuiHelper::ListBox(UI::Element& element)
{
    ImGui::Text(element.GetLabel().c_str());

    if (ImGui::BeginListBox(std::string("##" + element.GetLabel()).c_str())) {
        size_t size = element.GetComboList().size();
        for (int n = 0; n < size; n++) {
            const bool is_selected = (element.ComboInd == n);
            if (ImGui::Selectable(element.GetComboList()[n].c_str(), is_selected)) {
                element.ComboInd = n;
                if (n != 0) {
                    SelectedElement = element.GetComboList()[element.ComboInd];
                    IsDisplayInReset = true;
                }
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

}

void Core::ImGuiHelper::Tree(UI::Element& element)
{
    if (ImGui::TreeNode(element.GetLabel().c_str()))
    {
        for (const std::string& s : element.GetComboList()) {
            ImGui::Text(s.c_str());
        }

        ImGui::TreePop();
    }

}

void Core::ImGuiHelper::DebugImage(UI::Element& element)
{
    ImGui::Text(element.GetLabel().c_str());

    static bool active = true;
    if (DebugRT.empty() || DebugRT == "none") {
        return;
    }
    Gfx::RenderTargetsList id = Gfx::GetKey(DebugRT);
    Gfx::RenderTarget* rt = Gfx::Renderer::GetInstance()->GetRT(id);
    if (rt) {
        active = true;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 pos = viewport->Pos;
        ImGui::SetNextWindowPos(ImVec2(0, viewport->GetCenter().y), 0);
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin(DebugRT.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings );

        if (!rt->IsDepthSet()) {
            Gfx::Renderer::GetInstance()->Submit([&](VkCommandBuffer cmd) {
                rt->MemoryBarrier(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            });
        }

        ImGui::Image((ImTextureID)rt->GetImGuiDescriptor(), ImVec2(500, 400));
        ImGui::End();

        if (!active) {
            DebugRT = "none";
        }
    }
}

void Core::ImGuiHelper::Image(UI::Element& element)
{
    ImGui::Text(element.GetLabel().c_str());
    for (auto& it : Gfx::Renderer::GetInstance()->GetTextureMap()) {
        Gfx::RenderTarget* rt = Gfx::Renderer::GetInstance()->GetTexture(it.first);
        if (ImGui::ImageButton((ImTextureID)rt->GetImGuiDescriptor(), ImVec2(50, 50))) {
            Keeper::Objects* obj = ParseObjectID(SelectedElement);
            TextureUpdateInfo.OldTextureName = obj->GetTextureName();
            obj->SetTextureName(it.first);
            TextureUpdateInfo.NewTextureName = it.first;
            TextureUpdateInfo.ID = obj->GetID();
            IsDisplayInReset = true;
            printf("OBJECT [%d] INFO TEXTURE: new ->%s !!! old->%s\n",obj->GetID(), TextureUpdateInfo.NewTextureName.c_str(), TextureUpdateInfo.OldTextureName.c_str());
            TextureUpdate = true;
        }
        ImGui::SameLine();
    }
}

void Core::ImGuiHelper::ProcessUI(UI::Element& element)
{
    switch (element.GetType()) {
        case UI::TREE: {
            Tree(element);
            break;
        }
        case UI::IMAGE: {
            Image(element);
            break;
        }
        case UI::DEBUG_IMAGE: {
            DebugImage(element);
            break;
        }
        case UI::COMBO: {
            Combo(element);
            break;
        }
        case UI::LISTBOX: {
            ListBox(element);
            break;
        }
        case UI::FLOAT: {
            ImGui::Text((element.GetLabel() + ": %f").c_str(), element.DefinitionFloat());
            break;
        }
        case UI::BUTTON: {
            if (ImGui::Button(element.GetLabel().c_str())) {
                if (element.DefinitionFloat) {
                    element.DefinitionFloat();
                }
            }
            break;
        }
        case UI::CHECKBOX: {
            bool check = element.GetCheckbox();
            ImGui::Checkbox(element.GetLabel().c_str(), &check);
            element.DefinitionBool(check);
            element.SetCheckbox(check);
            break;
        }
        case UI::TEXT:
            if (element.DefinitionString) {
                ImGui::Text((element.GetLabel() + ": %s").c_str(), element.DefinitionString().c_str());
            }
            else {
                ImGui::TextUnformatted(element.GetLabel().c_str());
            }
            break;
        case UI::SEPARATOR:
            ImGui::Separator();
            break;
        case UI::SLIDER: {
            static float vol = 0.0;
            ImGui::SliderFloat(element.GetLabel().c_str(), &vol, 0.0f, 1.0f);
            if (element.DefinitionFloatArg) {
            element.DefinitionFloatArg(vol);
            }
            break;
        }
        default:
            break;
    }
}

void Core::ImGuiHelper::Render()
{
    ImGui::ShowDemoWindow();

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
    if (!SelectedElement.empty()) {
        auto it = std::find_if(UITabs.begin(), UITabs.end(), [](std::pair<UI::Element, std::vector<UI::Element>> pair) { return pair.first.GetLabel() == "Scene"; } );
        DisplayObjectInfo(SelectedElement, it->first);
    }
}
