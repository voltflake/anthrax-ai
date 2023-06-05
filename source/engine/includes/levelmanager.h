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
\nNPC:\n\
\tpath: \n\
\tx: \n\
\ty: \n\
\nTrigger:\n\
\tpath: \n\
\tcollision: \n\
\tvisible: \n\
\tx: \n\
\ty: \n";

struct Resources {
	char path[64] = "";
	int x = 0;
	int y = 0;
	int follow = 0;
	bool collision = 0;
	bool visible = 0;
};

struct NewLevel {
	Resources player;
	Resources camera;
	Resources background;
	Resources npc;
	Resources trigger;
};

class LevelManager {
public:
	void newlevel();
	void loadlevel();

	void savelevel();

	NewLevel level;
};