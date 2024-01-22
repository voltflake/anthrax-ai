#include "vkdefines.h"

const std::string levelinfo =
"Player:\n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n\
\nCamera:\n\
\tfollow: \n\
\nBackground:\n\
\tpath: \n\
\nTrigger:\n\
\tpath: \n\
\tcollision: \n\
\tvisible: \n\
\tx: \n\
\ty: \n\
\nObject:\n\
\tpath: \n\
\tcollision: \n\
\tx: \n\
\ty: \n";

struct Resources {
	char	path[64] = "";
	int 	x = 0;
	int 	y = 0;
	int 	follow = 0;
	bool 	collision = 0;
	bool 	visible = 0;

	int state = IDLE;
};

struct NewLevel {
	Resources player;
	Resources camera;
	Resources background;
	std::vector<Resources> trigger;
	std::vector<Resources> object;
	
	bool loaded = false;
	bool initres = false;

	int triggersize = 10;
	int objectsize = 10;
};

class LevelManager {
public:
	bool newlevel();
	bool loadlevel();

	void savelevel();

	NewLevel level;

	bool check = false;
	bool check2 = false;

};