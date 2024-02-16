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
		char path[64] = "";
		strcpy(path, getplayer()->getpath().c_str());
		Positions tmppos = getplayer()->getposition();

        ImGui::InputText("1path", path, 64);
		if (strcmp(path, getplayer()->getpath().c_str()) != 0) {
			getplayer()->setpath(path);
		}
	    ImGui::Columns(3);
    	ImGui::TextUnformatted("position:");
    	ImGui::NextColumn();
    	ImGui::InputInt("x", &tmppos.x);
    	ImGui::NextColumn();
    	ImGui::InputInt("y", &tmppos.y);
		if (tmppos.x != getplayer()->getposition().x || tmppos.y != getplayer()->getposition().y) {
			getplayer()->setposition(tmppos);
		}
    	ImGui::NextColumn();
    	ImGui::Columns(1);
       	ImGui::Separator();

    	ImGui::Checkbox("collision", &getplayer()->collision);
    	ImGui::Separator();

    }
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Checkbox("follow", &camera.follow);
        ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("Background")) {
		char path[64] = "";
		strcpy(path, getbackground().getpath().c_str());
        ImGui::InputText("2path", path, 64);
		if (strcmp(path, getbackground().getpath().c_str()) != 0) {
			getbackground().setpath(path);
		}
       	ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("Objects")) {
        if (ImGui::TreeNode("Trigger")) {
			int size = gettrigger().size();
        	ImGui::InputInt("trigger amount", &size);
			if (size != gettrigger().size()) {
				gettrigger().resize(size);
			}
			for (int i = 0; i < size; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					gettrigger()[i].ID = i;
					ImGui::Checkbox("visible", &gettrigger()[i].visible);
					ImGui::Checkbox("collision", &gettrigger()[i].collision);
					char path[64] = "";
					strcpy(path, gettrigger()[i].getpath().c_str());
					ImGui::InputText("3path", path, 64);
					if (strcmp(path, gettrigger()[i].getpath().c_str()) != 0) {
						gettrigger()[i].setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("trigger position:");
					ImGui::NextColumn();
					Positions tmppos = gettrigger()[i].getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					gettrigger()[i].setposition(tmppos);
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
			int size = getobject().size();
        	ImGui::InputInt("object amount", &size);
			if (size != getobject().size()) {
				getobject().resize(size);
			}
			for (int i = 0; i < size; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					getobject()[i].ID = i;
					ImGui::Checkbox("collision", &getobject()[i].collision);
					char path[64] = "";
					strcpy(path, getobject()[i].getpath().c_str());
					ImGui::InputText("4path", path, 64);
					if (strcmp(path, getobject()[i].getpath().c_str()) != 0) {
						getobject()[i].setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("object position:");
					ImGui::NextColumn();
					Positions tmppos = getobject()[i].getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					getobject()[i].setposition(tmppos);
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
    if (ImGui::Button("save level")){
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

    ImGui::Begin("LoadLevel", &open, ImGuiWindowFlags_NoResize);

	ImGui::InputText("Level:", filename, 64);

    std::string ff = "levels/";
    ff += std::string(filename);
	std::string test = filename;
	if (test.empty()) {
    	ImGui::End();
		return true;
	}
    std::string modify;

	int triggerind = 0;
	int objectind = 0;

	if (!loaded) {
		gettrigger().clear();
		getobject().clear();
		getplayer()->clear();
		getbackground().clear();

		if (std::ifstream in{ff}) {
			while (std::getline(in, modify)) {
				if (modify == "Trigger:") {
					triggerind++;
				}
				if (modify == "Object:") {
					objectind++;
				}
			}
		}

		if (triggerind != 0) {
			gettrigger().reserve(triggerind);
		}
		if (objectind != 0) {
			getobject().reserve(objectind);
		}
		std::ifstream in(ff);
    	while (std::getline(in, modify)) {
    		if (modify == "Player:") {
				std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			getplayer()->ID = stoi(modify);

    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
				getplayer()->setpath(modify.c_str());

				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			getplayer()->collision = (modify) != "0";

				int x, y;
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			y = stoi(modify);
				getplayer()->setposition({x, y});
    		}
    		if (modify == "Camera:") {
				std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			getcamera().ID = stoi(modify);

    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);

    			getcamera().follow = stoi(modify);
    		}
    		if (modify == "Background:") {
				std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			getbackground().ID = stoi(modify);

    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0,found);
    			getbackground().setpath(modify.c_str());
    		}
    		if (modify == "Trigger:") {
    			Resources tmp;

    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			tmp.ID = stoi(modify);

    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
				tmp.setpath(modify.c_str());

				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			tmp.collision = (modify) != "0";

				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			tmp.visible = (modify) != "0";

				int x, y;
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			y = stoi(modify);
				tmp.setposition({x, y});

				gettrigger().emplace_back(tmp);
    		}
    		if (modify == "Object:") {
    			Resources tmp;

    			std::getline(in, modify);
    			std::size_t found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			tmp.ID = stoi(modify);

    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
				tmp.setpath(modify.c_str());

				std::getline(in, modify);
				found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			tmp.collision = (modify) != "0";

				int x, y;
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			x = stoi(modify);
    			std::getline(in, modify);
    			found = modify.find_first_of(":") + 2;
    			modify.erase(0, found);
    			y = stoi(modify);
				tmp.setposition({x, y});

				getobject().emplace_back(tmp);
    		}
    	}
		in.close();
	}

	if (ImGui::Button("Load")){
		loaded = true;
    	initres = true;

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
    }
    else {
    	ff = "levels/0";
    }
	std::cout << "[FILE LOCATION AND NAME] " << ff << '\n';

  	std::ofstream outfile(ff);

  	std::string currline;
	std::string info;

	// player info
	outfile << playerinfo << std::endl;
	outfile << backgroundinfo << std::endl;
	for (int k = 0; k < gettrigger().size(); k++) {
		// trigger info
		outfile << triggerinfo << std::endl;
	}
	for (int k = 0; k < getobject().size(); k++) {
		// object info
		outfile << objectinfo << std::endl;
	}
	outfile.close();

	std::ifstream infile(ff);

	int triggerind = 0;
	int objind = 0;
  	while (std::getline(infile, currline)) {
		if (currline == "Player:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getplayer()->ID) + "\n";
			std::getline(infile, currline);
			info += currline + getplayer()->getpath() + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getplayer()->collision) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getplayer()->getposition().x) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getplayer()->getposition().y) + "\n";
		}
		if (currline == "Camera:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getcamera().ID) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getcamera().follow) + "\n";
		}
		if (currline == "Background:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getbackground().ID) + "\n";
			std::getline(infile, currline);
			info += currline + getbackground().getpath() + "\n";
		}
		if (currline == "Trigger:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].ID) + "\n";
			std::getline(infile, currline);
			info += currline + gettrigger()[triggerind].getpath() + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].collision) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].visible) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].getposition().x) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].getposition().y) + "\n";
			triggerind++;
		}
		if (currline == "Object:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind].ID) + "\n";
			std::getline(infile, currline);
			info += currline + getobject()[objind].getpath() + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind].collision) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind].getposition().x) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind].getposition().y) + "\n";
			objind++;
		}
	}
	infile.close();

  	std::ofstream outf(ff);
	outf << info << std::endl;
	outf.close();
}