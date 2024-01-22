#include "../includes/vkengine.h"

void Engine::collision(int& state, bool collision) {
    if (!collision) {
        return ;
    }
    float w, h;
	
	w = Builder.texturehandler.gettexture(Levels.level.player.path)->w;
	h = Builder.texturehandler.gettexture(Levels.level.player.path)->h;

    for (int i = 0; i < Levels.level.object.size(); i++) {
        if (Levels.level.player.y + h + h + 20 <= Levels.level.object[i].y) {
            state |= MOVE_DOWN;
        }
        else if (state & MOVE_DOWN) {
            state = IDLE;
        }
    }
}

void Engine::move() { 
    if (Levels.level.player.path != "") {
        collision(Levels.level.player.state, Levels.level.player.collision);
        if (Levels.level.player.state & MOVE_DOWN) {
            Levels.level.player.y += 5;
        }
        if (Levels.level.player.state & MOVE_UP) {
            Levels.level.player.y -= 20;
        }
        if (Levels.level.player.state & MOVE_LEFT) {
            Levels.level.player.x -= 5;  
        }
        if (Levels.level.player.state & MOVE_RIGHT) {
            Levels.level.player.x += 5;
        }
        
        Builder.updateplayer(Builder.getmesh(Levels.level.player.path), Levels.level.player.path, Levels.level.player.x, Levels.level.player.y);
	}
}