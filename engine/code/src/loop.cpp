#include "anthraxAI/vkengine.h"

void Engine::loop() {
	if (state & ENGINE_EDITOR) {
		ui();
		editor();
		update();
		update3d();
		render();
	}
	if (state & PLAY_GAME) {
		move();
		update();
		render();
	}
	if (state & EXIT) {
		cleanup();
	}
}