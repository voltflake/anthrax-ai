#include "vkdefines.h"
#include "parser.h"
#include <map>

const std::string playerinfo =
"Player:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n\
\tanimation: \n\
\tw: \n\
\th: \n";

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
\tanimation: \n\
\tw: \n\
\th: \n\
\ttype: \n";

const std::string objectinfo =
"Object:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n\
\tanimation: \n\
\tw: \n\
\th: \n";

class Player {
public:
	int 	ID = 0;
	bool 	collision = 0;
	bool 	animation = 0;
	int 	state = IDLE;
	bool 	debugcollision = false;

	void clear() {
		setpath("");
		setposition({0, 0});
		setanimsize({0, 0});
		collision = 0;
		animation = 0;
		state = IDLE;
	}

	Player() {};
	Player(const Player& pl) {
		ID = pl.ID;
		collision = pl.collision;
		animation = pl.animation;
		animsize = pl.animsize;
		state = pl.state;
		path = pl.path;
		pos = pl.pos;
	};

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };
	
	void setanimsize(Positions size) { animsize = size; };
	Positions getanimsize() { return animsize; };

	std::string getpath() { return path; };
	Positions getposition() { return pos; };

private:
	std::string path = "";
	Positions 	pos = {0, 0};
	Positions 	animsize = {0, 0};
};

class Camera {
public:
	int 	ID = 0;
	bool 	follow = 0;


	Camera() {};
	Camera(const Camera& cam) {
		ID = cam.ID;
		follow = cam.follow;
	};
};

class Resources {
public:
	int 	ID = 0;
	bool 	collision = 0;
	bool 	animation = 0;
	bool 	visible = 0;
	bool	move = 0;
	bool	display = 0;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		animation = 0;
		setanimsize({0, 0});
		visible = 0;
		display = 0;
		triggertype = TYPE_NONE;
	}

	Resources() {};
	Resources(const Resources& res) {
		ID = res.ID;
		collision = res.collision;
		animation = res.animation;
		visible = res.visible;
		move = res.move;
		path = res.path;
		pos = res.pos;
		animsize = res.animsize;
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

	void setanimsize(Positions size) { animsize = size; };
	Positions getanimsize() { return animsize; };

	void settextpath(std::string str) { textpath = str; };
	void settextposition(Positions postmp) { textpos = postmp; };
	void settextlist(std::map<int, std::vector<std::string>>& list) { textlist = list; };
	void settextind(int i) { textind = i; };

	std::string gettextpath() { return textpath; };
	std::map<int, std::vector<std::string>>& gettextlist() {  return textlist; };
	Positions gettextposition() { return textpos; };
	int gettextind() { return textind; };
private:
	TriggerType triggertype;
	std::string path = "";
	Positions 	pos = {0, 0};

	Positions 	animsize = {0, 0};

	std::map<int, std::vector<std::string>> textlist;
	Positions textpos = {0, 0};
	int textind = 0;
	std::string textpath = "text-output-scheme";
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

	Player* 				getplayer() {return &player; };
	Camera& 				getcamera() {return camera; };
	Resources& 				getbackground() {return background; };
	std::vector<Resources>& gettrigger() {return trigger; };
	std::vector<Resources>& getobject() {return object; };

	void 					setplayer(const Player& pl) { player = pl; };
	void 					setcamera(const Camera& cam) { camera = cam; };
	void 					setbackground(const Resources& bg) { background = bg; };
	void 					setobject(const std::vector<Resources>& obj);
	void 					settrigger(const std::vector<Resources>& tr);

	Parser parser;

private:
	Player player;
	Camera camera;
	
	Resources background;
	std::vector<Resources> trigger;
	std::vector<Resources> object;
};