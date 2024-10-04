#include "anthraxAI/vkengine.h"

void Engine::load2dresources()
{
    int triggersize = Level.gettrigger().size();
	int objectsize = Level.getobject().size();
	int k = 0;
	for (int i = 0; i < objectsize; i++) {
		if (Level.getobject()[i]->getpath() != "") {
            Resources.add(TYPE_OBJECT + k, 
	        Data(Level.getobject()[i]->getpath(), {Level.getobject()[i]->getposition().x, Level.getobject()[i]->getposition().y, 0}, Level.getobject()[i]->collision, Level.getobject()[i]->animation));
	
			Level.getobject()[i]->ID = k;
			k++;
		}
	}
	k = objectsize;
	// should be here ause i don't handle move/catch code for triggers yet
	for (int i = 0; i < triggersize; i++) {
		if (Level.gettrigger()[i].getpath() != "") {
            Resources.add(TYPE_OBJECT + k, 
	        Data(Level.gettrigger()[i].getpath(), {Level.gettrigger()[i].getposition().x, Level.gettrigger()[i].getposition().y, 0}, Level.gettrigger()[i].collision, Level.gettrigger()[i].animation));
			k++;
		}
	}
	if (Level.getplayer()->getpath() != "") {
        Resources.add(TYPE_PLAYER, 
	    Data(Level.getplayer()->getpath(), {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y, 0}, Level.getplayer()->collision, Level.getplayer()->animation));
		
	}
	if (Level.getbackground().getpath() != "") {
		Resources.add(TYPE_BACKGROUND, 
	    Data(Level.getbackground().getpath(), {0, 0, 0}, false, false));
	}

}

void Engine::load3dresources()
{
#ifdef AAI_WINDOWS
  Resources.add(TYPE_MODEL, 
	    Data("zeroone.png", {0, 0, -10}, false, false));
    Resources.add(TYPE_MODEL + 1, 
	    Data("bg2.png", {2, 0, -10}, false, false));
    Resources.add(TYPE_MODEL + 2, 
	    Data("floor.jpg", {3, 0, -5}, false, false));
#else
    Resources.add(TYPE_MODEL, 
	    Data("zeroone.png", {0, 0, -10}, false, false, false));
    Resources.add(TYPE_MODEL + 1, 
	    Data("bg2.png", {2, 0, -10}, false, false,  false));
    Resources.add(TYPE_MODEL + 2, 
	    Data("floor.jpg", {3, 0, -5}, false, false, false));
#endif
    Resources.add(TYPE_MODEL + 3,
	    Data("bg3.png", {5, 0, 0}, false, false));
    Resources.add(TYPE_MODEL + 4, 
	    Data("dummy.png", {8, 0, 0}, false, false));

    Resources.add(TYPE_MODEL + 3,
	    Data("bg3.png", {5, 0, 0}, false, false));
    Resources.add(TYPE_MODEL + 4, 
	    Data("dummy.png", {8, 0, 0}, false, false));

    Resources.add(TYPE_GIZMO + 0, 
	    Data("dummy.png", {0, 0, 0}, false, false));
    Resources.add(TYPE_GIZMO + 1, 
	    Data("dummy.png", {0, 0, 0}, false, false));
    Resources.add(TYPE_GIZMO + 2, 
	    Data("dummy.png", {0, 0, 0}, false, false));
}

void Engine::initresources()
{
    if (Level.test3d) {
		load3dresources();
    }
    else {
		load2dresources();
    }
}

void Engine::initmeshes()
{
	if (Level.test3d) {

		Builder.loadmodel("models/monkeytextured.obj", TYPE_MODEL + 0);
		Builder.loadmodel("models/cube.obj", TYPE_MODEL + 1);
		Builder.loadmodel("models/sphere.obj", TYPE_MODEL + 2);

		Builder.loadmodel("models/gizmox.obj", TYPE_GIZMO + 0);
		Builder.loadmodel("models/gizmoy.obj", TYPE_GIZMO + 1);
		Builder.loadmodel("models/gizmoz.obj", TYPE_GIZMO + 2);
		
		Debug.startms = getcurtime();
		Builder.loadmodel("models/Defeated.dae", TYPE_MODEL + 3);
		animator.init({"models/Defeated.dae", "models/body-jab-cross.dae", "models/double-kick-anim.dae"} , TYPE_MODEL + 3);
		
		Builder.loadmodel("models/Walking-mesh-anim.dae", TYPE_MODEL + 4);
		animator.init({"models/Walking-mesh-anim.dae", "models/Dying-anim.dae"} , TYPE_MODEL + 4);

		Debug.animprepared = true;
		
		for (int i = TYPE_MODEL; i < TYPE_MODEL + 5; i++) {
			Builder.updatemodel(Builder.getmodel(i));
		}
		for (int i = TYPE_GIZMO; i < TYPE_GIZMO + 3; i++) {
			Builder.updatemodel(Builder.getmodel(i));
		}
	}
    else {
    	Builder.loadmeshes();
    }
}