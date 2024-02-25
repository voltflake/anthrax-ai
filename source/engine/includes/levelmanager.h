#include "vkdefines.h"

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
\tvisible: \n\
\tx: \n\
\ty: \n";

const std::string objectinfo =
"Object:\n\
\tID: \n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n";

class Player {
public:
	int 	ID = 0;
	bool 	collision = 0;
	int 	state = IDLE;
	bool 	debugcollision = false;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		state = IDLE;
	}

	Player() {};
	Player(const Player& pl) {
		ID = pl.ID;
		collision = pl.collision;
		state = pl.state;
		path = pl.path;
		pos = pl.pos;
	};

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };
	
	std::string getpath() { return path; };
	Positions getposition() { return pos; };

private:
	std::string path = "";
	Positions 	pos = {0, 0};
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
	bool 	visible = 0;
	bool	move = 0;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		visible = 0;
	}

	Resources() {};
	Resources(const Resources& res) {
		ID = res.ID;
		collision = res.collision;
		visible = res.visible;
		move = res.move;
		path = res.path;
		pos = res.pos;
	};

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };
	
	std::string getpath() { return path; };
	Positions getposition() { return pos; };

private:
	std::string path = "";
	Positions 	pos = {0, 0};
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



private:
	Player player;
	Camera camera;
	
	Resources background;
	std::vector<Resources> trigger;
	std::vector<Resources> object;
};