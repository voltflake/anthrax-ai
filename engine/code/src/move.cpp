#include "anthraxAI/vkengine.h"

void Engine::update() {
    for (int i = 0; i < Level.getobject().size(); i++) {
        if (!Level.getobject()[i]->update) {
            continue;
        }
        Positions sizes;

        if (Level.getobject()[i]->animation) {
            sizes = Positions(int(Level.getobject()[i]->getanimator()[0].getanimsize().x), int(Level.getobject()[i]->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getobject()[i]->getpath())->w), int(Builder.gettexture(Level.getobject()[i]->getpath())->h) );
        }
        Builder.updatemesh(Builder.getmesh(TYPE_OBJECT + Level.getobject()[i]->ID), sizes, Level.getobject()[i]->getposition());
    }

    if (Level.getplayer()->update) {
        Positions sizes;
        if (Level.getplayer()->animation) {
            sizes = Positions(int(Level.getplayer()->getanimator()[0].getanimsize().x), int(Level.getplayer()->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getplayer()->getpath())->w), int(Builder.gettexture(Level.getplayer()->getpath())->h) );
            Level.getplayer()->update = false;
        }
        Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), sizes, Level.getplayer()->getposition());
    }
}

void Engine::move() {
    processanimation();
    processtrigger();
    moveplayer();
}

bool Engine::collision(int& state, bool collision, Positions pos, Positions sizes) {
    if (!collision || Level.getobject().empty()) {
        return true;
    }

    for (int i = 0; i < Level.getobject().size(); i++) {
        Positions objsizes;
        if (Level.getobject()[i]->animation) {
            objsizes = Positions(int(Level.getobject()[i]->getanimator()[0].getanimsize().x), int(Level.getobject()[i]->getanimator()[0].getanimsize().y) );
        }
        else {
            objsizes = Positions(int(Builder.gettexture(Level.getobject()[i]->getpath())->w), int(Builder.gettexture(Level.getobject()[i]->getpath())->h) );
        }
     
        if (pos.x < Level.getobject()[i]->getposition().x + objsizes.x &&
        pos.x + sizes.x > Level.getobject()[i]->getposition().x &&
        pos.y < Level.getobject()[i]->getposition().y + objsizes.y &&
        pos.y + sizes.y > Level.getobject()[i]->getposition().y ) {
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

        Positions sizes;
        if (Level.getplayer()->animation) {
           sizes = Positions(int(Level.getplayer()->getanimator()[0].getanimsize().x), int(Level.getplayer()->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getplayer()->getpath())->w), int(Builder.gettexture(Level.getplayer()->getpath())->h) );
        }
    
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y + 8}, sizes) && Level.getplayer()->state & MOVE_DOWN) {
            tmp.y += 8;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y - 32}, sizes) && Level.getplayer()->state & MOVE_UP) {
            tmp.y -= 16;
            Level.getplayer()->state ^= MOVE_UP;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x - 5, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_LEFT) {
            tmp.x -= 10;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x + 5, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_RIGHT) {
            tmp.x += 10;
        }
        if (tmp.x != tmp2.x || tmp.y != tmp2.y) {
            Level.getplayer()->setposition(tmp);
            Level.getplayer()->update = true;
            //Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), TYPE_PLAYER, Level.getplayer()->getposition());
        }
	}
}
