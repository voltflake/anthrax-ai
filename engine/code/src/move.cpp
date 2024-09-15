#include "anthraxAI/vkengine.h"


void Engine::updatebones(int id) {
    if (Debug.animprepared) {
        float timesec = (float)((double)getcurtime() - (double)Debug.startms) / 1000.0f;
		std::vector<glm::mat4> vec = animator.getbonestransform(Builder.getmodel(id), id, timesec * Debug.animspeed);

        for(int i = 0; (i < vec.size() ); i++) {
            storagedata.bonesmatrices[i] = Builder.getmodel(id)->fintransforms[i];//vec[i];
        }

        char* storagetmp;
        vkMapMemory(Builder.getdevice(), Builder.getmodel(id)->storagebuffer.devicememory, 0, BONE_ARRAY_SIZE, 0, (void**)&storagetmp);
        memcpy( storagetmp, &storagedata.bonesmatrices, (size_t)BONE_ARRAY_SIZE);
        vkUnmapMemory(Builder.getdevice(), Builder.getmodel(id)->storagebuffer.devicememory);
    }
}

void Engine::preparecamerabuffer() {
    glm::mat4 view = glm::lookAt(EditorCamera.getposition(), EditorCamera.getposition() + EditorCamera.getfront(), EditorCamera.getup());
	glm::mat4 projection = glm::perspective(glm::radians(45.f), float(Builder.getswapchainextent().width) / float(Builder.getswapchainextent().height), 0.01f, 100.0f);
	projection[1][1] *= -1;

	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;
	camdata.viewpos = glm::vec4(EditorCamera.getposition(), 1.0);
	camdata.mousepos = {Mouse.pos.x, Mouse.pos.y, 0, 0};
	camdata.viewport = {WindowExtend.width, WindowExtend.height, 0, 0};
    camdata.dir_light_pos = glm::vec4(DirectionLight.position, 1.0);
    camdata.dir_light_color = glm::vec4(DirectionLight.color, 1.0);

    for (int i = 0; i < pointlightamount; i++) {
        camdata.point_light_pos[i] = glm::vec4(PointLights[i].position, 1.0);
        camdata.point_light_color[i] = glm::vec4(PointLights[i].color, 1.0);
    }
   camdata.pointlightamount = pointlightamount;

	char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(CameraData));
  	vkMapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory, 0, sceneParamBufferSize, 0, (void**)&datadst);
   	int frameind = FrameIndex % MAX_FRAMES_IN_FLIGHT;
	datadst += Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * frameind;
    memcpy( datadst, &camdata, (size_t)sizeof(CameraData));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory);

}


void Engine::mousepicking() {

    if (Mouse.state != MOUSE_PRESSED) {
        return;
    }
    
    if ( state & PLAY_GAME) {
        std::vector<RenderObject>& rq = Builder.getrenderqueue();
        std::vector<RenderObject>::iterator model_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.selected == true && val.type == TYPE_MODEL; } );;
        if (model_it != rq.end()) {
            model_it->selected = false;;
            gizmomove.visible = false;
        }
        return;
    }

    void* storagetmp;
    VkDeviceSize storagesize = sizeof(u_int )* DEPTH_ARRAY_SCALE;
    vkMapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory, BONE_ARRAY_SIZE, storagesize, 0, (void**)&storagetmp);
    u_int* u = static_cast<u_int*>(storagetmp);

    int selectedID = -1;
    for (int i = 0; i < DEPTH_ARRAY_SCALE; i++) {
        if (u[i] != 0) {
            selectedID = u[i];
            Mouse.state = MOUSE_SELECTED;
            break;
        }

    }
    std::memset(storagetmp, 0, DEPTH_ARRAY_SCALE * sizeof(uint32_t));
    vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory);

    bool selected = false;
    RenderObject* rq = Builder.getrenderqueue().data();
    for (int i = 0; i < Builder.getrenderqueue().size(); i++)
    {
        RenderObject& object = rq[i];

        if ((object.type == TYPE_MODEL || object.type == TYPE_GIZMO)) {
            if (object.ID == selectedID) {
                object.selected = true;
                selected = true;
                if (object.type != TYPE_GIZMO && object.type != TYPE_LIGHT) {
                    gizmomove.objecthandler = selectedID;
                    gizmomove.visible = true;
                }
                else if (object.type != TYPE_LIGHT) {
                    gizmomove.axis = static_cast<GizmoAxis>(object.ID);
                }
            }
            else {
                object.selected = false;
            }
        }
    }
    if (!selected) {
        gizmomove.visible = false;
    }
}

void Engine::update3d() {
	   
    std::vector<RenderObject>& rq = Builder.getrenderqueue();
    
    std::vector<RenderObject>::iterator gizmo_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == AXIS_Z; } );
    if (gizmo_it == rq.end()) {
        return;
    }

    std::vector<RenderObject>::iterator light_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == TYPE_LIGHT; } );
    if (light_it == rq.end()) {
        return;
    }
    light_it->pos.x = camdata.dir_light_pos.x;
    light_it->pos.y = camdata.dir_light_pos.y;
    light_it->pos.z = camdata.dir_light_pos.z;

    for (int i = 0; i < pointlightamount; i++) {
        std::vector<RenderObject>::iterator pointlight_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == TYPE_LIGHT + i + 1; } );
        if (pointlight_it == rq.end()) {
            return;
        }
        pointlight_it->pos.x = camdata.point_light_pos[i].x;
        pointlight_it->pos.y = camdata.point_light_pos[i].y;
        pointlight_it->pos.z = camdata.point_light_pos[i].z;
    }

    std::vector<RenderObject>::iterator model_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == gizmomove.objecthandler && val.type == TYPE_MODEL; } );;
    if (model_it == rq.end()) {
        return;
    }

    if ((Mouse.posdelta.x < -1 || Mouse.posdelta.x > 1) && gizmomove.axis == AXIS_X) {
        model_it->pos.x -= 0.01*  Mouse.posdelta.x ;
    }
    if ((Mouse.posdelta.y < -1 || Mouse.posdelta.y > 1) && gizmomove.axis == AXIS_Y) {
        model_it->pos.y += 0.01 *  Mouse.posdelta.y ;
    }
    if ((Mouse.posdelta.y < -1 || Mouse.posdelta.y > 1) && gizmomove.axis == AXIS_Z) {
        model_it->pos.z += 0.01 *  Mouse.posdelta.y;
    }
    model_it->selected = true;
   // printf("1: %d | %d------------------\n", gizmo_it->ID, gizmo_it->type);
    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;
    gizmo_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == AXIS_Y; } );   
  //  printf("2: %d | %d------------------\n", gizmo_it->ID, gizmo_it->type);

    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;
    //++gizmo_it;
    gizmo_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == AXIS_X; } );   

   // printf("3: %d | %d------------------\n", gizmo_it->ID, gizmo_it->type);

    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;

}

void Engine::update() {
    if (Level.test3d) return;

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
    if (Level.test3d) return;
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
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x - 2, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_LEFT) {
            tmp.x -= 4;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x + 2, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_RIGHT) {
            tmp.x += 4;
        }
        if ((tmp.x != tmp2.x || tmp.y != tmp2.y) && (tmp.x > 0 && tmp.x < WindowExtend.width) && (tmp.y > 0 && tmp.y < WindowExtend.height)) {
            Level.getplayer()->setposition(tmp);
            Level.getplayer()->update = true;
            //Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), TYPE_PLAYER, Level.getplayer()->getposition());
        }
	}
}
