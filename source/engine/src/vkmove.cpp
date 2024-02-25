#include "../includes/vkengine.h"

bool Engine::collision(int& state, bool collision, Positions pos) {
    if (!collision || Level.getobject().empty()) {
        return true;
    }
    float w, h;

	w = Builder.texturehandler.gettexture(Level.getplayer()->getpath())->w;
	h = Builder.texturehandler.gettexture(Level.getplayer()->getpath())->h;

    for (int i = 0; i < Level.getobject().size(); i++) {

        float objw = Builder.texturehandler.gettexture(Level.getobject()[i].getpath())->w;
        float objh = Builder.texturehandler.gettexture(Level.getobject()[i].getpath())->h;
        if (pos.x < Level.getobject()[i].getposition().x + objw &&
        pos.x + w > Level.getobject()[i].getposition().x &&
        pos.y < Level.getobject()[i].getposition().y + objh &&
        pos.y + h > Level.getobject()[i].getposition().y ) {
            return true;
        }
    }
    state |= MOVE_DOWN;
    return false;
}

void Engine::moveplayer() {
    if (Level.getplayer()->getpath() != "") {
        Positions tmp = Level.getplayer()->getposition();
        Positions tmp2 = tmp;

        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y + 16}) && Level.getplayer()->state & MOVE_DOWN) {
            tmp.y += 16;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y - 16}) && Level.getplayer()->state & MOVE_UP) {
            tmp.y -= 16;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x - 5, Level.getplayer()->getposition().y}) && Level.getplayer()->state & MOVE_LEFT) {
            tmp.x -= 10;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x + 5, Level.getplayer()->getposition().y}) && Level.getplayer()->state & MOVE_RIGHT) {
            tmp.x += 10;
        }
        if (tmp.x != tmp2.x || tmp.y != tmp2.y) {
            Level.getplayer()->setposition(tmp);
            Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), TYPE_PLAYER, Level.getplayer()->getposition());
        }
	}
}

bool Engine::editormove() {
    if (mousestate != MOUSE_MOVE || !freemove || Level.getobject().empty()) {
        return true ;
    }
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i].getpath() == "") {
            continue;
        }
        if (Level.getobject()[i].move) {
            Builder.updatemesh(Builder.getmesh(TYPE_OBJECT + Level.getobject()[i].ID), TYPE_OBJECT + Level.getobject()[i].ID, mousepos);
            Level.getobject()[i].setposition(mousepos);
            return true ;
        }
    }
    return false ;
}

void Engine::uncatchobject() {
    if (mousestate != MOUSE_RELEASED || !freemove || Level.getobject().empty()) {
        return ;
    }
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i].getpath() == "") {
            continue;
        }
        if (Level.getobject()[i].move) {
            Level.getobject()[i].move = false;
            break ;
        }
    }
}

void Engine::catchobject() {
    if (mousestate != MOUSE_PRESSED || !freemove || Level.getobject().empty()) {
        return ;
    }

    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i].getpath() == "") {
            continue;
        }
        float objw = Builder.texturehandler.gettexture(Level.getobject()[i].getpath())->w;
        float objh = Builder.texturehandler.gettexture(Level.getobject()[i].getpath())->h;
        // std::cout << "---------------------\n";
        // std::cout << (mousepos.x < Level.getobject()[i].getposition().x + objw) << "\n";
        // std::cout << (mousepos.x > Level.getobject()[i].getposition().x) << "\n";
        // std::cout << (mousepos.y < Level.getobject()[i].getposition().y + objh) << "\n";
        // std::cout << (mousepos.y > Level.getobject()[i].getposition().y) << "\n";

        // std::cout << "---------------------\n";
        if (mousepos.x < Level.getobject()[i].getposition().x + objw &&
            mousepos.x > Level.getobject()[i].getposition().x &&
            mousepos.y < Level.getobject()[i].getposition().y + objh &&
            mousepos.y > Level.getobject()[i].getposition().y ) {
            // std::cout << (mousepos.x ) << "!X!" << Level.getobject()[i].getposition().x<< "\n";
            // std::cout << (mousepos.y ) << "!Y!" << Level.getobject()[i].getposition().y << "\n";

            std::cout << "mouse catched object [" << Level.getobject()[i].getpath() << "]\n";
            Level.getobject()[i].move = true;
            mousestate = MOUSE_MOVE;
            break;
            // Builder.updatemesh(Builder.getmesh(TYPE_OBJECT + Level.getobject()[i].ID), TYPE_OBJECT, mousepos);
            // Level.getobject()[i].setposition(mousepos);
        }
   }
}