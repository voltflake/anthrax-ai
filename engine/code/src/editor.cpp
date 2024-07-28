#include "anthraxAI/vkengine.h"

void Engine::editor() {
    catchobject();
    editormove();
    uncatchobject();
}

bool Engine::editormove() {
    if (Mouse.state != MOUSE_MOVE || !Debug.freemove || Level.getobject().empty()) {
        return true ;
    }
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->getpath() == "") {
            continue;
        }
        if (Level.getobject()[i]->freemove) {
            Positions tmp = Level.getobject()[i]->getposition();
            int factorx = (Mouse.begin.x - Mouse.pos.x) ;
            int factory = (Mouse.begin.y - Mouse.pos.y) ;
            Positions finpos = {(tmp.x - factorx), ((tmp.y - factory))};
            Level.getobject()[i]->setposition(finpos);
            Level.getobject()[i]->update = true;
            Mouse.begin = Mouse.pos;
            return true ;
        }
    }
    return false ;
}

void Engine::uncatchobject() {
    if (state & PLAY_GAME || Mouse.state != MOUSE_RELEASED || !Debug.freemove || Level.getobject().empty()) {
        return ;
    }
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->getpath() == "") {
            continue;
        }
        if (Level.getobject()[i]->freemove) {
            Level.getobject()[i]->freemove = false;
            break ;
        }
    }
}

void Engine::catchobject() {
    if (state & PLAY_GAME || Mouse.state != MOUSE_PRESSED || !Debug.freemove || Level.getobject().empty()) {
        return ;
    }

    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->getpath() == "") {
            continue;
        }
        float objw = Builder.texturehandler.gettexture(Level.getobject()[i]->getpath())->w;
        float objh = Builder.texturehandler.gettexture(Level.getobject()[i]->getpath())->h;
        // std::cout << "---------------------\n";
        // std::cout << (mousepos.x < Level.getobject()[i]->getposition().x + objw) << "\n";
        // std::cout << (mousepos.x > Level.getobject()[i]->getposition().x) << "\n";
        // std::cout << (mousepos.y < Level.getobject()[i]->getposition().y + objh) << "\n";
        // std::cout << (mousepos.y > Level.getobject()[i]->getposition().y) << "\n";
        // std::cout << "---------------------\n";
        if (Mouse.pos.x < Level.getobject()[i]->getposition().x + objw &&
            Mouse.pos.x > Level.getobject()[i]->getposition().x &&
            Mouse.pos.y < Level.getobject()[i]->getposition().y + objh &&
            Mouse.pos.y > Level.getobject()[i]->getposition().y ) {
            // std::cout << (Mouse.pos.x ) << "!X!" << objw<< "\n";
            // std::cout << (Mouse.pos.y ) << "!Y!" << objh << "\n";
            std::cout << "mouse catched object [" << Level.getobject()[i]->getpath() << "]\n";
            Level.getobject()[i]->freemove = true;
            Level.getobject()[i]->update = true;
            Mouse.state = MOUSE_MOVE;
            break;
        }
   }
}