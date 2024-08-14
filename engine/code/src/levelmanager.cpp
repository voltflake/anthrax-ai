#include "anthraxAI/levelmanager.h"
#include <filesystem>
#include <fstream>

bool LevelManager::newlevel() {
	bool open = true;

	// const ImGuiViewport* viewport = ImGui::GetMainViewport();
    // const ImVec2 base_pos = viewport->Pos;
    // ImGui::SetNextWindowPos(ImVec2(base_pos.x + 0, base_pos.y + 390), ImGuiCond_Once);
	// ImGui::SetNextWindowSize(ImVec2(500, 450), 0);

    // ImGui::Begin("NewLevel", &open, ImGuiWindowFlags_NoResize);

	Player playertmp = *getplayer();
	Resources bgtmp =  getbackground();
	CameraRes camtmp = getcamera();
	std::vector<Resources> trigtmp = gettrigger();
	std::vector<Resources*> objtmp = getobject();

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
    	ImGui::Checkbox("animation", &playertmp.animation);
    	ImGui::Checkbox("debug collision", &playertmp.debugcollision);
    	ImGui::Separator();
		setplayer(playertmp);

    }
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Checkbox("follow", &camtmp.follow);
        ImGui::Separator();
		setcamera(camtmp);
	}
    if (ImGui::CollapsingHeader("Background")) {
		char path[64] = "";
		strcpy(path, bgtmp.getpath().c_str());
        ImGui::InputText("2path", path, 64);
		if (strcmp(path, bgtmp.getpath().c_str()) != 0) {
			bgtmp.setpath(path);
		}
       	ImGui::Separator();
		setbackground(bgtmp);
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
					ImGui::Checkbox("collision", &trigtmp[i].collision);
					char path[64] = "";
					strcpy(path, trigtmp[i].getpath().c_str());
					ImGui::InputText("3path", path, 64);
					if (strcmp(path, trigtmp[i].getpath().c_str()) != 0) {
						trigtmp[i].setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3, "trigger info");
					ImGui::TextUnformatted("trigger position:");
					ImGui::NextColumn();
					Positions tmppos = trigtmp[i].getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					trigtmp[i].setposition(tmppos);
					ImGui::NextColumn();
					ImGui::Columns(1);
					
					static int item = trigtmp[i].gettriggertype();
					static const char* triggerarr[] = {
						GetValue(TYPE_NONE),
						GetValue(TYPE_TEXT),
						GetValue(TYPE_ACTION),
					};
					ImGui::Combo("trigger type", &item, triggerarr, IM_ARRAYSIZE(triggerarr));
					if (static_cast<TriggerType>(item) != trigtmp[i].gettriggertype()) {
						trigtmp[i].settriggertype(static_cast<TriggerType>(item));
					}
					if (trigtmp[i].gettriggertype() == TYPE_TEXT) {
						char patht[64] = "";
						strcpy(patht, trigtmp[i].gettextpath().c_str());
						ImGui::InputText("5path", patht, 64);
						if (strcmp(patht, trigtmp[i].gettextpath().c_str()) != 0) {
							trigtmp[i].settextpath(patht);
						}
						ImGui::Separator();
						ImGui::Columns(3, "text info");
						ImGui::TextUnformatted("text position:");
						ImGui::NextColumn();
						Positions tmppost = trigtmp[i].gettextposition();
						ImGui::InputInt("x1", &tmppost.x);
						ImGui::NextColumn();
						ImGui::InputInt("y1", &tmppost.y);
						trigtmp[i].settextposition(tmppost);
						ImGui::NextColumn();
						ImGui::Columns(1);
					}
					ImGui::TreePop();
					ImGui::Spacing();
				}
			}
            ImGui::TreePop();
            ImGui::Spacing();
			settrigger(trigtmp);
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
					objtmp[i]->ID = i;
					ImGui::Checkbox("collision", &objtmp[i]->collision);
					ImGui::Checkbox("animation", &objtmp[i]->animation);
					if (!objtmp[i]->animation) {
						objtmp[i]->update = false;
					}
					char path[64] = "";
					strcpy(path, objtmp[i]->getpath().c_str());
					ImGui::InputText("4path", path, 64);
					if (strcmp(path, objtmp[i]->getpath().c_str()) != 0) {
						objtmp[i]->setpath(path);
					}
					ImGui::Separator();
					ImGui::Columns(3);
					ImGui::TextUnformatted("object position:");
					ImGui::NextColumn();
					Positions tmppos = objtmp[i]->getposition();
					ImGui::InputInt("x", &tmppos.x);
					ImGui::NextColumn();
					ImGui::InputInt("y", &tmppos.y);
					objtmp[i]->setposition(tmppos);
					ImGui::NextColumn();
					ImGui::Columns(1);

					ImGui::TreePop();
					ImGui::Spacing();
				}
			}
            ImGui::TreePop();
            ImGui::Spacing();
			setobject(objtmp);
        }
        ImGui::Separator();
    }
		
    if (ImGui::Button("save level")) {
    	savelevel();
    }

//	ImGui::End();

	if (!open) { return false; };

	return true;
}

bool LevelManager::loadlevel() {

	static char filename[64] = "";
	bool open = true;

	ImGui::InputText("Level:", filename, 64);

    std::string ff = "levels/";
    ff += std::string(filename);
	std::string test = filename;
	if (test.empty()) {
		return true;
	}

	if (test == "1") {
		test3d = true;
	}
	else {
		test3d = false;
	}
    std::string modify;

	int triggerind = 0;
	int objectind = 0;
	ImGui::SameLine();
	if (ImGui::Button("Load")){
		std::ifstream in(ff.c_str());
    	if (!in.good()) {
			return true;
		}

		gettrigger().clear();
		for (int i = 0; i < getobject().size(); i++) {
			delete getobject()[i];
		}
		getobject().clear();
		getplayer()->clear();
		getbackground().clear();
		parser.Clear();

		if (in) {
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

		parser.Load(ff);
		if (parser.GetElement("Player:")) {
			getplayer()->ID = parser.GetElement<int>(PARSE_ID);
			getplayer()->setpath(parser.GetElement<std::string>(PARSE_PATH));
			getplayer()->collision = parser.GetElement<bool>(PARSE_COLLISION);
			Positions pos;
			pos.x = parser.GetElement<int>(PARSE_X);
			pos.y = parser.GetElement<int>(PARSE_Y);
			getplayer()->setposition(pos);
			if (parser.GetElement("\tanimation:")) {
				getplayer()->animation = 1;
				Animator2d anim;
				anim.ID = parser.GetElement<int>(PARSE_ID);
				anim.setpath(parser.GetElement<std::string>(PARSE_PATH));
				Positions sizes;
				sizes.x = parser.GetElement<int>(PARSE_X);
				sizes.y = parser.GetElement<int>(PARSE_Y);
				anim.setanimsize(sizes);
				anim.setfps(parser.GetElement<float>(PARSE_FPS));
				anim.settype(GetKey<AnimationType>(parser.GetElement<std::string>(PARSE_TYPE)));
				getplayer()->pushanimator(anim);
			}
		}
		if (parser.GetElement("Background:")) {
   			getbackground().ID = parser.GetElement<int>(PARSE_ID);
			getbackground().setpath(parser.GetElement<std::string>(PARSE_PATH));
		}
		if (parser.GetElement("Camera:")) {
			getcamera().ID = parser.GetElement<int>(PARSE_ID);
			getcamera().follow = parser.GetElement<bool>(PARSE_FOLLOW);
		}
		while (parser.GetElement("Trigger:")) {
    		Resources tmp;

			tmp.ID = parser.GetElement<int>(PARSE_ID);
			tmp.setpath(parser.GetElement<std::string>(PARSE_PATH));
			tmp.collision = parser.GetElement<bool>(PARSE_COLLISION);
			Positions pos;
			pos.x = parser.GetElement<int>(PARSE_X);
			pos.y = parser.GetElement<int>(PARSE_Y);
			tmp.setposition(pos);
			tmp.animation = parser.GetElement<bool>(PARSE_ANIMATION);
			if (tmp.animation) {
				// Positions sizes;
				// sizes.x = parser.GetElement<int>(PARSE_W);
				// sizes.y = parser.GetElement<int>(PARSE_H);
				// tmp.setanimsize(sizes);
			}
			tmp.settriggertype(GetKey<TriggerType>(parser.GetElement<std::string>(PARSE_TYPE)));
			
			if (tmp.gettriggertype() == TYPE_TEXT) {
				tmp.settextpath(parser.GetElement<std::string>(PARSE_TEXT));
				Positions post;
				post.x = parser.GetElement<int>(PARSE_X);
				post.y = parser.GetElement<int>(PARSE_Y);
				tmp.settextposition(post);
			}
			gettrigger().emplace_back(tmp);
		}
		while (parser.GetElement("Object:")) {
			Resources* tmp = new Resources;

			tmp->ID = parser.GetElement<int>(PARSE_ID);
			tmp->setpath(parser.GetElement<std::string>(PARSE_PATH));
			tmp->collision = parser.GetElement<bool>(PARSE_COLLISION);
			Positions pos;
			pos.x = parser.GetElement<int>(PARSE_X);
			pos.y = parser.GetElement<int>(PARSE_Y);
			tmp->setposition(pos);
			tmp->animation = parser.GetElement<bool>(static_cast<Elements>(PARSE_ANIMATION - 2));
			if (tmp->animation) {

				if (parser.GetElement("\tanimation: 1")) {
					tmp->animation = 1;
					Animator2d anim;
					anim.ID = parser.GetElement<int>(PARSE_ID);
					anim.setpath(parser.GetElement<std::string>(PARSE_PATH));
					Positions sizes;
					sizes.x = parser.GetElement<int>(PARSE_X);
					sizes.y = parser.GetElement<int>(PARSE_Y);
					anim.setanimsize(sizes);
					anim.setfps(parser.GetElement<float>(PARSE_FPS));

					anim.settype(GetKey<AnimationType>(parser.GetElement<std::string>(PARSE_TYPE)));

					tmp->pushanimator(anim);
				}
			}
			getobject().emplace_back(tmp);
		}
    	initres = true;

		std::cout << filename << "\n";
		std::string sss = "check";
    }

	if (!open) { return false; };
	return true;
}

void LevelManager::savelevel() {

    std::string path = "./levels/";

    std::vector<std::string> files;

    for (auto& entry : std::filesystem::directory_iterator(path)) {
    	if (entry.path() != ""){
       		files.push_back(entry.path().generic_string());
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
			std::getline(infile, currline);
			info += currline + std::to_string(getplayer()->animation) + "\n";
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
			info += currline + std::to_string(gettrigger()[triggerind].getposition().x) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].getposition().y) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(gettrigger()[triggerind].animation) + "\n";
			std::getline(infile, currline);
			info += currline + GetValue(gettrigger()[triggerind].gettriggertype()) + "\n";
			triggerind++;
		}
		if (currline == "Object:") {
			info += currline + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind]->ID) + "\n";
			std::getline(infile, currline);
			info += currline + getobject()[objind]->getpath() + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind]->collision) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind]->getposition().x) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind]->getposition().y) + "\n";
			std::getline(infile, currline);
			info += currline + std::to_string(getobject()[objind]->animation) + "\n";
			objind++;
		}
	}
	infile.close();

  	std::ofstream outf(ff);
	outf << info << std::endl;
	outf.close();
}

void LevelManager::setobject(const std::vector<Resources*>& obj) { 
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
