#include "../includes/vkengine.h"

void Engine::start() {
	ASSERT(state != INIT_ENGINE, "How is it possible?");
	linuxinitwindow();
	init();
	state = ENGINE_EDITOR;
}

int main(int argc, char **argv) {
	Engine engine;
	
#ifdef OS_WINDOWS
		engine.wininitwindow();
		engine.init();
		engine.runwindows();
		engine.cleanup();
#endif

#ifdef OS_LINUX
		engine.start();
		engine.runlinux();
#endif

	return 0;
}

