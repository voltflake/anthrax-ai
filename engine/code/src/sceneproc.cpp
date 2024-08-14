#include "anthraxAI/vkbuilder.h"

void VkBuilder::procscene(ResourcesMap& resmap, Animator& animator) 
{
    int i = 0;

	for (auto& list : resmap) {
		if (list.second.texturepath == "") {
            continue;
        }
		RenderObject obj;

        DataTypes type = static_cast<DataTypes>(list.first);
        std::string material;
        bool anim = false;
        int id = -1;

        if (list.first >= TYPE_MODEL) {
			obj.model = getmodel(list.first);
            type = list.first >= TYPE_GIZMO ? TYPE_GIZMO : TYPE_MODEL;
			id = list.first;
            material = "monkey";
        }
        else {
			obj.mesh = getmesh(list.first);
            material = "defaultmesh";
        }
        if (animator.hasanimation(list.first)) {
            material = "animated";
            anim = true;
            id = list.first;
        }

		descriptors.updatesamplerdescriptors(list.second.texturepath);

        obj.type = type;
        obj.material = getmaterial(material);
        obj.animated = anim;
        obj.ID = id;

		obj.pos = list.second.pos;
		obj.textureset = &descriptors.getmainsamplerdescriptor()[i];
		obj.debugcollision = list.second.debugcollision;
		pushrenderobject(obj);
		i++;
    }

	RenderObject debug;
	debug.material = getmaterial("debug");
	debug.debug = true;
	pushrenderobject(debug);
}
