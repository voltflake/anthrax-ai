#include "../includes/levelmanager.h"
#include <filesystem>
#include <fstream>

bool LevelManager::newlevel() {
	bool open = true;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 215), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 450), 0);

    ImGui::Begin("NewLevel", &open, ImGuiWindowFlags_NoResize);

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
    	ImGui::Checkbox("show collision", &visible);

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
        if (ImGui::TreeNode("Trigger")) {
			int size = level.trigger.size();
        	ImGui::InputInt("amount", &size);        	
			if (size != level.trigger.size()) {
				level.trigger.resize(size);
			}
			for (int i = 0; i < size + 1; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					ImGui::Checkbox("visible", &level.trigger[i].visible);
					ImGui::Checkbox("collision", &level.trigger[i].collision);
					ImGui::InputText("4Path",    level.trigger[i].path, 64);
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("Position:");
					ImGui::NextColumn();
					ImGui::InputInt("x", &level.trigger[i].x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &level.trigger[i].y);
					ImGui::NextColumn();
					ImGui::Columns(1);

					ImGui::TreePop();
					ImGui::Spacing();
				}
			}
            ImGui::TreePop();
            ImGui::Spacing();
        }
        if (ImGui::TreeNode("Object")) {
        	
			int size = level.object.size();
        	ImGui::InputInt("amount", &size);        	
			if (size != level.object.size()) {
				level.object.resize(size);
			}
			for (int i = 0; i < size + 1; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					ImGui::Checkbox("collision", &level.object[i].collision);
					ImGui::InputText("5Path",    level.object[i].path, 64);
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("Position:");
					ImGui::NextColumn();
					ImGui::InputInt("x", &level.object[i].x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &level.object[i].y);
					ImGui::NextColumn();
					ImGui::Columns(1);

					ImGui::TreePop();
					ImGui::Spacing();
				}
			}
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
	ImGui::SetNextWindowSize(ImVec2(500, 80), 0);
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 130), ImGuiCond_Once);

    ImGui::Begin("LoadLevel", &open,  ImGuiWindowFlags_NoResize);

	ImGui::InputText("Level:", filename, 64);
    std::string modify;

    std::string ff = "levels/";
    ff += std::string(filename);

	static int triggerind = 0;
	static int objectind = 0;

	if (!level.loaded) {
			level.trigger.clear();
	level.object.clear();
	std::cout << "!!!!!!!!!!!!!" << level.loaded << '\n';

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
    		if (modify == "Trigger:") {
    			Resources tmp;

    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(tmp.path, modify.c_str());
				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.collision = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.visible = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.y = stoi(modify);
				
				//if (triggerind < level.trigger.size()) {
					level.trigger.emplace_back(tmp);
				//}
				triggerind++;
    		}
    		if (modify == "Object:") {
    			Resources tmp;
    			
    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			strcpy(tmp.path,modify.c_str());
				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.collision = (modify)  != "0";
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			tmp.y = stoi(modify);
				//if (objectind - 1 < level.object.size()) {
					level.object.emplace_back(tmp);
				//}
				objectind++;
    		}
    	}
    	level.loaded = true;
    	level.initres = true;

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
		level.loaded = false;
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

	int triggerind = 0;
	int objectind = 0;

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
    		if (modify == "Trigger:") {
				files.push_back(modify);
				
				std::getline(in, modify);
				files.push_back(modify + level.trigger[triggerind].path);
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.trigger[triggerind].collision));
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.trigger[triggerind].visible));
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.trigger[triggerind].x));
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.trigger[triggerind].y));

				triggerind++;
				if (triggerind < level.trigger.capacity()) {
					files.push_back("Trigger:");
				}
    		}
    		if (modify == "Object:") {
				files.push_back(modify);
				
				std::getline(in, modify);
				files.push_back(modify + level.object[objectind].path);
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.object[objectind].collision));
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.object[objectind].x));
				std::getline(in, modify);
				files.push_back(modify + std::to_string(level.object[objectind].y));

				objectind++;
				std::cout << level.trigger.capacity() << '\n';
				if (objectind < level.object.capacity()) {
					files.push_back("Object:");
				}
    			std::getline(in, modify);
    		}
    	}
	}
 	if (std::ofstream out{ff}) {
		for (auto& s : files){
			out << s << std::endl;
		}
	}
}