#include "../includes/vkengine.h"

void Engine::editor() {
    catchobject();
    editormove();
    uncatchobject();
}

bool Engine::editormove() {
    if (mousestate != MOUSE_MOVE || !freemove || Level.getobject().empty()) {
        return true ;
    }
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (Level.getobject()[i]->getpath() == "") {
            continue;
        }
        if (Level.getobject()[i]->freemove) {
            Positions tmp = Level.getobject()[i]->getposition();
            int factorx = (mousebegin.x - mousepos.x) ;
            int factory = (mousebegin.y - mousepos.y) ;
            Positions finpos = {(tmp.x - factorx), ((tmp.y - factory))};
            Level.getobject()[i]->setposition(finpos);
            Level.getobject()[i]->update = true;
            mousebegin = mousepos;
            return true ;
        }
    }
    return false ;
}

void Engine::uncatchobject() {
    if (state & PLAY_GAME || mousestate != MOUSE_RELEASED || !freemove || Level.getobject().empty()) {
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
    if (state & PLAY_GAME || mousestate != MOUSE_PRESSED || !freemove || Level.getobject().empty()) {
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
        if (mousepos.x < Level.getobject()[i]->getposition().x + objw &&
            mousepos.x > Level.getobject()[i]->getposition().x &&
            mousepos.y < Level.getobject()[i]->getposition().y + objh &&
            mousepos.y > Level.getobject()[i]->getposition().y ) {
            // std::cout << (mousepos.x ) << "!X!" << objw<< "\n";
            // std::cout << (mousepos.y ) << "!Y!" << objh << "\n";
            std::cout << "mouse catched object [" << Level.getobject()[i]->getpath() << "]\n";
            Level.getobject()[i]->freemove = true;
            Level.getobject()[i]->update = true;
            mousestate = MOUSE_MOVE;
            break;
        }
   }
}