#include "anthraxAI/editorui.h"

void EditorUI::beginwindow(const Positions& offset, const Positions& size, ImGuiWindowFlags flags,
                            const std::string& name, bool* act)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x + offset.x, base_pos.y + offset.y), 0);
	if (size.x != 0 && size.y != 0){
        ImGui::SetNextWindowSize(ImVec2(size.x, size.y), ImGuiCond_FirstUseEver);
    }
    static bool active = true;
	if (act) {
    	ImGui::Begin(name.c_str(), act, flags);
    }
    else {
    	ImGui::Begin(name.c_str(), &active, flags);
    }
}

void EditorUI::endwindow()
{
	ImGui::End();
}

// void EditorUI::animator2d()
// {
//     beginwindow( { 500, 40 }, { 500, 800 }, ImGuiCond_FirstUseEver, "Vulkan Texture Test");

//     // const ImGuiViewport* viewport = ImGui::GetMainViewport();
//     // const ImVec2 base_pos = viewport->Pos;
//     // ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 500, base_pos.y + 40), 0);
// 	// ImGui::SetNextWindowSize(ImVec2(500, 800), ImGuiCond_FirstUseEver);
	
// 	// ImGui::Begin("Vulkan Texture Test");
// 	for (int i = 0; i < Debug.ImGuiAnim.size(); i++) {
// 		ImGui::TextUnformatted(Debug.ImGuiAnim[i].path.c_str());
// 		ImGui::SliderInt(std::string("scale " + std::to_string(i)).c_str(), &Debug.ImGuiAnim[i].scale, 1, 8);
// 		ImGui::Image((ImTextureID)Debug.ImGuiAnim[i].desc, ImVec2(Builder.gettexture(Debug.ImGuiAnim[i].path)->w / Debug.ImGuiAnim[i].scale, Builder.gettexture(Debug.ImGuiAnim[i].path)->h / Debug.ImGuiAnim[i].scale));
// 		ImGui::Separator();
// 	}

//     endwidnow();
// 	// ImGui::End();
// }