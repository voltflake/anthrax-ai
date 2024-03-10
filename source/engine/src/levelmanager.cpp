#include "../includes/levelmanager.h"
#include <filesystem>
#include <fstream>

bool LevelManager::newlevel() {
	bool open = true;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 390), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 450), 0);

    ImGui::Begin("NewLevel", &open, ImGuiWindowFlags_NoResize);

	Player playertmp = *getplayer();
	Resources bgtmp =  getbackground();
	Camera camtmp = getcamera();
	std::vector<Resources> trigtmp = gettrigger();
	std::vector<Resources> objtmp = getobject();

 	if (ImGui::CollapsingHeader("Player")) {
		char path[64] = "";
		strcpy(path, playertmp.getpath().c_str());
		Positions tmppos = playertmp.getposition();

        ImGui::InputText("1path", path, 64);
		if (strcmp(path, playertmp.getpath().c_str()) != 0) {
			playertmp.setpath(path);
		}
	    ImGui::Columns(3);
    	ImGui::TextUnformatted("position:");
    	ImGui::NextColumn();
    	ImGui::InputInt("x", &tmppos.x);
    	ImGui::NextColumn();
    	ImGui::InputInt("y", &tmppos.y);
		if (tmppos.x != playertmp.getposition().x || tmppos.y != playertmp.getposition().y) {
			playertmp.setposition(tmppos);
		}
    	ImGui::NextColumn();
    	ImGui::Columns(1);
       	ImGui::Separator();

    	ImGui::Checkbox("collision", &playertmp.collision);
    	ImGui::Checkbox("debug collision", &playertmp.debugcollision);
    	ImGui::Separator();

    }
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Checkbox("follow", &camtmp.follow);
        ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("Background")) {
		char path[64] = "";
		strcpy(path, bgtmp.getpath().c_str());
        ImGui::InputText("2path", path, 64);
		if (strcmp(path, bgtmp.getpath().c_str()) != 0) {
			bgtmp.setpath(path);
		}
       	ImGui::Separator();
    }
    if (ImGui::CollapsingHeader("Objects")) {
        if (ImGui::TreeNode("Trigger")) {
			int size = trigtmp.size();
        	ImGui::InputInt("trigger amount", &size);
			if (size != trigtmp.size()) {
				trigtmp.resize(size);
			}
			for (int i = 0; i < size; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					trigtmp[i].ID = i;
					ImGui::Checkbox("visible", &trigtmp[i].visible);
					ImGui::Checkbox("collision", &trigtmp[i].collision);
					char path[64] = "";
					strcpy(path, trigtmp[i].getpath().c_str());
					ImGui::InputText("3path", path, 64);
					if (strcmp(path, trigtmp[i].getpath().c_str()) != 0) {
						trigtmp[i].setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("trigger position:");
					ImGui::NextColumn();
					Positions tmppos = trigtmp[i].getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					trigtmp[i].setposition(tmppos);
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
			int size = objtmp.size();
        	ImGui::InputInt("object amount", &size);
			if (size != objtmp.size()) {
				objtmp.resize(size);
			}
			for (int i = 0; i < size; i++) {
				std::string name = "[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(name.c_str())) {
					objtmp[i].ID = i;
					ImGui::Checkbox("collision", &objtmp[i].collision);
					char path[64] = "";
					strcpy(path, objtmp[i].getpath().c_str());
					ImGui::InputText("4path", path, 64);
					if (strcmp(path, objtmp[i].getpath().c_str()) != 0) {
						objtmp[i].setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("object position:");
					ImGui::NextColumn();
					Positions tmppos = objtmp[i].getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					objtmp[i].setposition(tmppos);
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
    if (ImGui::Button("save level")) {
		setplayer(playertmp);
		setbackground(bgtmp);
		setcamera(camtmp);
		setobject(objtmp);
		settrigger(trigtmp);

		playertmp = *getplayer();
		bgtmp = getbackground();
		camtmp = getcamera();
		
		objtmp.clear(); 
		objtmp.reserve(getobject().size());
		for (auto& o : getobject()) { 
			objtmp.push_back(o);
		}
		trigtmp.clear(); 
		trigtmp.reserve(gettrigger().size());
		for (auto& o : gettrigger()) { 
			trigtmp.push_back(o);
		}
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
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 300), ImGuiCond_Once);

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

void LevelManager::setobject(const std::vector<Resources>& obj) { 
	object.clear(); 
	object.reserve(obj.size());
	for (auto& o : obj) { 
		object.push_back(o);
	} 
}

void LevelManager::settrigger(const std::vector<Resources>& tri) { 
	trigger.clear(); 
	trigger.reserve(tri.size());
	for (auto& o : tri) { 
		trigger.push_back(o);
	} 
}
