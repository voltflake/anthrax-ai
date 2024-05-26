#include "anthraxAI/vkengine.h"

void Engine::processanimation() {
    if (Level.getplayer()->animation) {
        if (Level.getplayer()->getanimator()[0].getoffset() == 0) {
            Level.getplayer()->getanimator()[0].setoffset(Builder.gettexture( Level.getplayer()->getpath())->w / Level.getplayer()->getanimator()[0].getanimsize().x);
        }
        Mesh* mesh = Builder.getmesh(TYPE_PLAYER);
        mesh->vertices[0].uv = {Level.getplayer()->getanimator()[0].getframe(), 0.0f};
        mesh->vertices[1].uv = {Level.getplayer()->getanimator()[0].getframe(), 1.0f};
        mesh->vertices[2].uv = {Level.getplayer()->getanimator()[0].getoffset() + Level.getplayer()->getanimator()[0].getframe(), 1.0f};
        mesh->vertices[3].uv = {Level.getplayer()->getanimator()[0].getoffset() + Level.getplayer()->getanimator()[0].getframe(), 0.0f};
        
        if (Level.getplayer()->getanimator()[0].getframecounter() > Level.getplayer()->getanimator()[0].getfps()) {
            Level.getplayer()->getanimator()[0].setframecounter(0);

            Level.getplayer()->getanimator()[0].setframe(Level.getplayer()->getanimator()[0].getframe() + Level.getplayer()->getanimator()[0].getoffset() );
            if (Level.getplayer()->getanimator()[0].getframe() >= 1.0f) {
                Level.getplayer()->getanimator()[0].setframe(0.0f);
            }
        }
        
        Level.getplayer()->getanimator()[0].setframecounter(Level.getplayer()->getanimator()[0].getframecounter() + 1);
    }

    for (int i = 0; i < Level.getobject().size(); i++) {
        if (!Level.getobject()[i]->animation) {
            continue;
        }
        if (Level.getobject()[i]->getanimator()[0].getoffset() == 0) {
            Level.getobject()[i]->getanimator()[0].setoffset(Builder.gettexture( Level.getobject()[i]->getpath())->w / Level.getobject()[i]->getanimator()[0].getanimsize().x);
        }
        Mesh* mesh = Builder.getmesh(TYPE_OBJECT + Level.getobject()[i]->ID);
        mesh->vertices[0].uv = {Level.getobject()[i]->getanimator()[0].getframe(), 0.0f};
        mesh->vertices[1].uv = {Level.getobject()[i]->getanimator()[0].getframe(), 1.0f};
        mesh->vertices[2].uv = {Level.getobject()[i]->getanimator()[0].getoffset() + Level.getobject()[i]->getanimator()[0].getframe(), 1.0f};
        mesh->vertices[3].uv = {Level.getobject()[i]->getanimator()[0].getoffset() + Level.getobject()[i]->getanimator()[0].getframe(), 0.0f};
        
        if (Level.getobject()[i]->getanimator()[0].getframecounter() > Level.getobject()[i]->getanimator()[0].getfps()) {
            Level.getobject()[i]->getanimator()[0].setframecounter(0);

            Level.getobject()[i]->getanimator()[0].setframe(Level.getobject()[i]->getanimator()[0].getframe() + Level.getobject()[i]->getanimator()[0].getoffset() );
            if (Level.getobject()[i]->getanimator()[0].getframe() >= 1.0f) {
                Level.getobject()[i]->getanimator()[0].setframe(0.0f);
            }
        }
        
        Level.getobject()[i]->getanimator()[0].setframecounter(Level.getobject()[i]->getanimator()[0].getframecounter() + 1);
        Level.getobject()[i]->update = true;
    }

    //     Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), TYPE_PLAYER, Level.getplayer()->getposition());
    //     //Level.getplayer()->update = false;
    // }
}