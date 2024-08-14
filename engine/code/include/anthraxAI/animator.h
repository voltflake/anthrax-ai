#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkmesh.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct AnimationData {

	std::string selectedpath;
	std::vector<std::string> animpaths;
	int pathindex = 0;
	int sceneind = 0;
};

class Animator {

public:
    Animator() {};
	
	Assimp::Importer importer[2];
	const aiScene* scene[10] = {NULL};

	glm::mat4 globinverse;
	std::map<int, AnimationData> animations;

	void init(std::vector<std::string> paths, int id) {

		animations[id].animpaths = paths;
		animations[id].selectedpath = paths[0];
		animations[id].pathindex = 0;
		animations[id].sceneind = animations.size() - 1;
		scene[animations[id].sceneind]  = importer[animations[id].sceneind].ReadFile(animations[id].selectedpath, IMPORT_PROPS);
		
		globinverse =  glm::inverse(glm::mat4(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0)); 
		globinverse *= rot;
	}
	
	glm::mat4 interpolatepos(glm::vec3 out, float timeticks, const aiNodeAnim* animnode);
	u_int findpos(float timeticks, const aiNodeAnim* animnode);
	glm::mat4 interpolaterot(glm::quat out, float timeticks, const aiNodeAnim* animnode);
	u_int findrot(float timeticks, const aiNodeAnim* animnode);
	glm::mat4 interpolatescale(glm::vec3 out, float timeticks, const aiNodeAnim* animnode);
	u_int findscale(float timeticks, const aiNodeAnim* animnode);

	bool hasanimation(int id) {
		return animations.find(id) != animations.end();
	}

	void reload(int id) {
		animations[id].selectedpath = animations[id].animpaths[animations[id].pathindex];
		scene[animations[id].sceneind] = importer[animations[id].sceneind].ReadFile(animations[id].selectedpath, IMPORT_PROPS);
	}
 
	const aiNodeAnim* findanim(const aiAnimation* anim, const std::string nodename);
	void readnodehierarchy(Model* model, int animid, const aiNode* node, float timetick, const glm::mat4 parenttransform);
    std::vector<glm::mat4> getbonestransform(Model* model, int animid, float time);

};

class Animator2d {
	public:
	int ID;
    void settype(AnimationType t) { type = t; };
    AnimationType gettype() { return type; };

	void setpath(std::string str) { path = str; };
	void setposition(Positions postmp) { pos = postmp; };

	void setoffset(float offs) { offset = 1.0f / offs; };
	
	void setanimsize(Positions size) { animsize = size; };
	Positions getanimsize() { return animsize; };

	std::string getpath() { return path; };
	Positions getposition() { return pos; };

	void setfps(float fps) { framecounterlim = MAX_FPS / fps;};
	float getfps() { return framecounterlim;};

	float getoffset() { return offset; };

	float getframe() { return frame; };
	void setframe(float f) { frame = f; };
	
	float getframecounter() { return framecounter; };
	void setframecounter(float f) { framecounter = f; };

	private:
	std::string path = "";
	Positions 	pos = {0, 0};
	Positions 	animsize = {0, 0};
    AnimationType type;

    float offset = 0.0f;
	float frame = 0.0f;
	float framecounter = 0.0f;
	float framecounterlim = 0.0f;
};
