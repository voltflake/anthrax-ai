#include "../includes/vkengine.h"

int main(int argc, char **argv)
{
	Engine engine;
	
#ifdef OS_WINDOWS
		engine.wininitwindow();
		engine.init();
		engine.runwindows();
		engine.cleanup();
#endif

#ifdef OS_LINUX
		engine.linuxinitwindow();
		engine.init();
		engine.runlinux();
		engine.cleanup();
#endif

	return 0;
}

