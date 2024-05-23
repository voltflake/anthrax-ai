#include "anthraxAI/vkengine.h"

void Engine::start() {
	ASSERT(state != INIT_ENGINE, "How is it possible?");
	init();
	state = ENGINE_EDITOR;
}

int main(int argc, char **argv) {
	Engine engine;
	engine.start();
	engine.run();
	engine.cleanup();
	return 0;
}

