#include "anthraxAI/vkengine.h"

void Engine::game_loop() {
	if (state & ENGINE_EDITOR) {
		ui();
		editor();
		update();
		draw();
	}
	if (state & PLAY_GAME) {
		move();
		update();
		draw();
	}
	if (state & EXIT) {
		cleanup();
	}
}