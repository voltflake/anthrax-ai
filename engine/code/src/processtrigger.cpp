#include "anthraxAI/vkengine.h"

void Engine::processtextind() {
    for (int i = 0; i < Level.gettrigger().size(); i++) {
        if (Level.gettrigger()[i].display) {
            if (Level.gettrigger()[i].gettextind() == Level.gettrigger()[i].gettextlist().size() - 1) {
                Level.gettrigger()[i].display = false;
                for (auto& elem : Level.gettrigger()[i].gettextlist()) {
                    elem.second.clear();
                }
                Level.gettrigger()[i].gettextlist().clear();
                return;
            }
            Level.gettrigger()[i].settextind(Level.gettrigger()[i].gettextind() + 1);
        }
    }
}

void Engine::processtext() {
    if (Level.gettrigger().empty()) {
        return ;
    }

    ImGuiStyle& style = ImGui::GetStyle();
	style = Debug.TextDisplayStyle;

    static bool active = false;
    for (int i = 0; i < Level.gettrigger().size(); i++) {
        if (Level.gettrigger()[i].display) {
            active = true;
            if (active) {
                // float oldscale = ImGui::GetFont()->Scale;
                // ImGui::GetFont()->Scale *= 1.2;
                // ImGui::PushFont(ImGui::GetFont());

                // const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(ImVec2(Level.gettrigger()[i].gettextposition().x, Level.gettrigger()[i].gettextposition().y), 0);
                ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
               
                ImGui::Begin("test", &active, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoDecoration);
                
                if (Level.gettrigger()[i].gettextlist().empty()) {
                    Level.gettrigger()[i].settextind(0);

                    std::ifstream read;
                    read.open("./texts/" + Level.gettrigger()[i].gettextpath());
                    std::string modify;
                    std::map<int, std::vector<std::string>> list;
                    int ind = 0;
                    while (std::getline(read, modify)) {
                        if (modify == ">") {
                            ind++;
                            std::getline(read, modify);
                        }
                        list[ind].push_back(modify);
                    }
                    read.close();
                    Level.gettrigger()[i].settextlist(list);
                }
                
                for (std::string& item : Level.gettrigger()[i].gettextlist()[Level.gettrigger()[i].gettextind()]) {
                    ImGui::TextUnformatted(item.c_str());
                }
                // ImGui::GetFont()->Scale = oldscale;
                // ImGui::PopFont();
                ImGui::End();
                return ;
            }
        }
    }
    active = false;
}

void Engine::processaction() {
    processtext();
}

bool Engine::processtrigger() {
    if (Level.gettrigger().empty()) {
        return false;
    }
    float w, h;

	w = Builder.texturehandler.gettexture(Level.getplayer()->getpath())->w;
	h = Builder.texturehandler.gettexture(Level.getplayer()->getpath())->h;

    Positions pos = Level.getplayer()->getposition();
    for (int i = 0; i < Level.gettrigger().size(); i++) {

        float objw = Builder.texturehandler.gettexture(Level.gettrigger()[i].getpath())->w;
        float objh = Builder.texturehandler.gettexture(Level.gettrigger()[i].getpath())->h;
        if (pos.x < Level.gettrigger()[i].getposition().x + objw &&
        pos.x + w > Level.gettrigger()[i].getposition().x &&
        pos.y < Level.gettrigger()[i].getposition().y + objh &&
        pos.y + h > Level.gettrigger()[i].getposition().y ) {
           // std::cout << "TRIGGER [" << Level.gettrigger()[i].gettextpath() << "]\n";
            if (Level.gettrigger()[i].gettriggertype() == TYPE_TEXT && Level.gettrigger()[i].gettextind() == 0) {
                Level.gettrigger()[i].display = true;
            }
        }
    }
	processaction();
    return false;
}
