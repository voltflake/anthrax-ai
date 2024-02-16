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

// const std::string levelinfo =
// "Player:\n\
// \tID: \n\
// \tpath: \n\
// \tcollision: \n\
// \tx: \n\
// \ty: \n\
// \nCamera:\n\
// \tID: \n\
// \tfollow: \n\
// \nBackground:\n\
// \tID: \n\
// \tpath: \n\
// \nTrigger:\n\
// \tID: \n\
// \tpath: \n\
// \tcollision: \n\
// \tvisible: \n\
// \tx: \n\
// \ty: \n\
// \nObject:\n\
// \tID: \n\
// \tpath: \n\
// \tcollision: \n\
// \tx: \n\
// \ty: \n";

// struct Resources {
// 	char	path[64] = "";
// 	int 	x = 0;
// 	int 	y = 0;
// 	int 	follow = 0;
// 	bool 	collision = 0;
// 	bool 	visible = 0;

// 	int state = IDLE;
// };

// struct NewLevel {
// 	Resources player;
// 	Resources camera;
// 	Resources background;
// 	std::vector<Resources> trigger;
// 	std::vector<Resources> object;
	
// 	bool loaded = false;
// 	bool initres = false;

// 	int triggersize = 0;
// 	int objectsize = 0;
// };

class Player {
public:
	int 	ID = 0;
	bool 	collision = 0;
	int state = IDLE;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		state = IDLE;
	}

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
};

class Resources {
public:
	int 	ID = 0;
	
	bool 	collision = 0;
	bool 	visible = 0;

	void clear() {
		setpath("");
		setposition({0, 0});
		collision = 0;
		visible = 0;
	}

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


private:
	Player player;
	Camera camera;
	
	Resources background;
	std::vector<Resources> trigger;
	std::vector<Resources> object;
};