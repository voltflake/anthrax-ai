#include "vkdefines.h"
#include "animator.h"
#include "parser.h"
#include <map>

const std::string playerinfo =
"Player:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n";

const std::string camerainfo =
"Camera:\n\
\tID: \n\
\tfollow: \n";

const std::string backgroundinfo =
"Background:\n\
\tID: \n\
\tpath: \n";

const std::string triggerinfo =
"Trigger:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n\
\ttype: \n";

const std::string objectinfo =
"Object:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n";

const std::string animinfo =
"animation:\n\
\t\tID: \n\
\t\tpath: \n\
\t\ttype: \n\
\t\tx: \n\
\t\ty: \n\
\t\tfps: \n";

class Resources {
public:
	int 	ID = 0;
	bool 	collision = 0;
	bool 	animation = 0;
	bool 	visible = 0;
	bool	freemove = 0;
	bool	display = 0;

	bool update = false;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		visible = 0;
		display = 0;
		triggertype = static_cast<TriggerType>(TYPE_NONE);
		update = false;
	}

	Resources() {};
	Resources(const Resources& res) {
		ID = res.ID;
		collision = res.collision;
		animation = res.animation;
		visible = res.visible;
		freemove = res.freemove;
		path = res.path;
		pos = res.pos;
		textpath = res.textpath;
		textpos = res.textpos;
		display = res.display;
		triggertype = res.triggertype;
	};

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };
	void settriggertype(TriggerType type) { triggertype = type; };
	TriggerType gettriggertype() { return triggertype; };

	std::string getpath() { return path; };
	Positions getposition() { return pos; };

	void settextpath(std::string str) { textpath = str; };
	void settextposition(Positions postmp) { textpos = postmp; };
	void settextlist(std::map<int, std::vector<std::string>>& list) { textlist = list; };
	void settextind(int i) { textind = i; };

	std::string gettextpath() { return textpath; };
	std::map<int, std::vector<std::string>>& gettextlist() {  return textlist; };
	Positions gettextposition() { return textpos; };
	int gettextind() { return textind; };

	std::vector<Animator2d>& getanimator() { return animations; }
	void pushanimator(Animator2d& anim) { animations.push_back(anim); }

private:
	TriggerType triggertype;
	std::string path = "";
	Positions 	pos = {0, 0};

	std::map<int, std::vector<std::string>> textlist;
	Positions textpos = {0, 0};
	int textind = 0;
	std::string textpath = "text-output-scheme";

	std::vector<Animator2d> animations;
};

class Player : public Resources {
public:
	int 	state = IDLE;
	bool 	debugcollision = false;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		animation = 0;
		state = IDLE;
		update = false;
	};

	Player() {};
	Player(const Player& pl) {
		ID = pl.ID;
		collision = pl.collision;
		animation = pl.animation;
		state = pl.state;
	};
private:
};

class CameraRes {
public:
	int 	ID = 0;
	bool 	follow = 0;


	CameraRes() {};
	CameraRes(const CameraRes& cam) {
		ID = cam.ID;
		follow = cam.follow;
	};
};


class LevelManager {
public:
	bool newlevel();
	bool loadlevel();

	void savelevel();

	bool loaded = false;
	bool initres = false;
	
	bool check = false;
	bool check2 = false;

	bool 						test3d = false;


	Player* 				getplayer() {return &player; };
	CameraRes& 				getcamera() {return camera; };
	Resources& 				getbackground() {return background; };
	std::vector<Resources>& gettrigger() {return trigger; };
	std::vector<Resources*>& getobject() {return object; };

	void 					setplayer(const Player& pl) { player = pl; };
	void 					setcamera(const CameraRes& cam) { camera = cam; };
	void 					setbackground(const Resources& bg) { background = bg; };
	void 					setobject(const std::vector<Resources*>& obj);
	void 					settrigger(const std::vector<Resources>& tr);

	Parser parser;

private:
	Player player;
	CameraRes camera;
	
	Resources background;
	std::vector<Resources> trigger;
	std::vector<Resources*> object;
};