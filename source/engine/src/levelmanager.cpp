#include "../includes/levelmanager.h"
#include <filesystem>
#include <fstream>

bool LevelManager::newlevel() {
	bool open = true;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 210), 0);
	ImGui::SetNextWindowSize(ImVec2(500, 450), ImGuiCond_FirstUseEver);

    ImGui::Begin("NewLevel", &open, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

 	if (ImGui::CollapsingHeader("Player")) {
        ImGui::InputText("1Path",     level.player.path, 64);
		static bool visible = false;

	    ImGui::Columns(3);
    	ImGui::TextUnformatted("Position:");
    	ImGui::NextColumn();
    	ImGui::InputInt("x", &level.player.x);
    	ImGui::NextColumn();
    	ImGui::InputInt("y", &level.player.y);
    	ImGui::NextColumn();
    	ImGui::Columns(1);
       	ImGui::Separator();

    	ImGui::Checkbox("collision", &level.player.collision);
    	ImGui::Checkbox("show", &visible);

    	ImGui::Separator();

    }
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::InputText("Follow",     level.camera.path, 64);
        ImGui::TextUnformatted("Size");
        ImGui::Separator();

    }
    if (ImGui::CollapsingHeader("Background")) {
        ImGui::InputText("2Path",     level.background.path, 64);
       	ImGui::Separator();

    }
    if (ImGui::CollapsingHeader("Objects")) {

 		if (ImGui::TreeNode("NPC")) {
        	ImGui::InputText("3Path",    level.npc.path, 64);

        	ImGui::Separator();
        	ImGui::Columns(3);
        	ImGui::TextUnformatted("Position:");
        	ImGui::NextColumn();
        	ImGui::InputInt("x", &level.npc.x);
        	ImGui::NextColumn();
        	ImGui::InputInt("y", &level.npc.y);
        	ImGui::NextColumn();
        	ImGui::Columns(1);

            ImGui::TreePop();
            ImGui::Spacing();
        }
        	ImGui::Separator();

        if (ImGui::TreeNode("Trigger")) {
        	
        	ImGui::Checkbox("visible", &level.trigger.visible);
       		ImGui::Checkbox("collision", &level.trigger.collision);
        	ImGui::InputText("4Path",    level.trigger.path, 64);
        	ImGui::Separator();
        	ImGui::Columns(3);
        	ImGui::TextUnformatted("Position:");
        	ImGui::NextColumn();
        	ImGui::InputInt("x", &level.trigger.x);
        	ImGui::NextColumn();
        	ImGui::InputInt("y", &level.trigger.y);
        	ImGui::NextColumn();
        	ImGui::Columns(1);

            ImGui::TreePop();
            ImGui::Spacing();
        }
        if (ImGui::TreeNode("Object")) {
        	
       		ImGui::Checkbox("collision", &level.object.collision);
        	ImGui::InputText("5Path",    level.object.path, 64);
        	ImGui::Separator();
        	ImGui::Columns(3);
        	ImGui::TextUnformatted("Position:");
        	ImGui::NextColumn();
        	ImGui::InputInt("x", &level.object.x);
        	ImGui::NextColumn();
        	ImGui::InputInt("y", &level.object.y);
        	ImGui::NextColumn();
        	ImGui::Columns(1);

            ImGui::TreePop();
            ImGui::Spacing();
        }
        	ImGui::Separator();

    }
    if (ImGui::Button("Save Level")){
    	savelevel();
    }

	ImGui::End();
	
	if (!open) { return false; };
	
	return true;
}

bool LevelManager::loadlevel() {

	static char filename[64] = "";
	bool open = true;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
	ImGui::SetNextWindowSize(ImVec2(500, 80), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 120), 0);

    ImGui::Begin("LoadLevel", &open, ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

	ImGui::InputText("Level:", filename, 64);
    std::string modify;

    std::string ff = "levels/";
    ff += std::string(filename);

	if (std::ifstream in{ff}) {
    	while (std::getline(in, modify)) {
    		if (modify == "Player:") {
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			strcpy(level.player.path,modify.c_str());

				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);

    			level.player.collision = (modify) != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			level.player.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			level.player.y = stoi(modify);
    		}
    		if (modify == "Camera:") {    			
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.camera.follow = stoi(modify);
    		}
    		if (modify == "Background:") {
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(level.background.path,modify.c_str());
    		}
    		if (modify == "NPC:") {
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(level.npc.path,modify.c_str());
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.npc.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.npc.y = stoi(modify);
    		}
    		if (modify == "Trigger:") {
    			
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(level.trigger.path,modify.c_str());
				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.trigger.collision = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.trigger.visible = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.trigger.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.trigger.y = stoi(modify);
    		}
    		if (modify == "Object:") {
    			
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(level.object.path,modify.c_str());
				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.object.collision = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.object.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			level.object.y = stoi(modify);
    		}
    	}
	}
	
	if (ImGui::Button("Load")){
		 std::cout << filename << "\n";
		 std::string sss = "check";
	    if (filename == sss) {
	    	for (const auto & entry : std::filesystem::directory_iterator("./textures/check/")) {
		    	if (entry.path() == ""){
		    		return true;
		    	}
		    }
	    	check = true;
	    	std::cout << "check passed\n";
	    	return true;
	    }

    	level.loaded = true;
    }

    ImGui::End();

	if (!open) { return false; };
	return true;
}

void LevelManager::savelevel() {

    std::string path = "./levels/";

    std::vector<std::string> files;

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
    	if (entry.path() != ""){
       		files.push_back(entry.path());
    	}
    }

	std::string ff;

    if (!files.empty()) {
 		std::sort(files.begin(), files.end());

    	ff = files[files.size() - 1];

   		int num = ff[ff.size() - 1] - '0';

		num++;

    	ff[ff.size() - 1] = num + '0';

    	std::ofstream outfile(ff);

		outfile << levelinfo << std::endl;

		outfile.close();
    }
    else {
    	ff = "levels/0";
	   	std::ofstream outfile2(ff);
		outfile2 << levelinfo << std::endl;
		outfile2.close();
    }
	
	files.clear();
    std::string modify;

 	if (std::ifstream in{ff}) {
    	while (std::getline(in, modify)) {
    		if (modify == "Player:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + level.player.path);
				std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.player.collision));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.player.x));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.player.y));
    		}
    		if (modify == "Camera:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.camera.follow));
    		}
    		if (modify == "Background:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + level.background.path);
    		}
    		if (modify == "NPC:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + level.npc.path);
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.npc.x));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.npc.y));
    		}
    		if (modify == "Trigger:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + level.trigger.path);
				std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.trigger.collision));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.trigger.visible));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.trigger.x));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.trigger.y));
    		}
    		if (modify == "Object:") {
    			files.push_back(modify);
    			
    			std::getline(in, modify);
    			files.push_back(modify + level.object.path);
				std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.object.collision));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.object.x));
    			std::getline(in, modify);
    			files.push_back(modify + std::to_string(level.object.y));
    		}
    	}
	}
 	if (std::ofstream out{ff}) {
		for (auto& s : files){
			out << s << std::endl;
		}
	}
}