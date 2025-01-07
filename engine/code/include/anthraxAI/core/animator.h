#pragma once

#include "anthraxAI/gfx/model.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "glm/fwd.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Core 
{
  struct AnimationData {
  
  	std::string CurrentPath;
  	std::vector<std::string> Paths;
  	int PathIndex = 0;
  	int SceneInd = 0;
  };
  
  class AnimatorBase {
  
    public:
        AnimatorBase() {};
  	    void Init();
        std::vector<glm::mat4> Update(Gfx::RenderObject& object);
        
        bool HasAnimation(int id) {
    		return Animations.find(id) != Animations.end();
    	}

    private:
    	Assimp::Importer Importer[2];
    	const aiScene* Scenes[10] = {NULL};
        
        glm::mat4 GlobalInverse; 
    	std::map<int, AnimationData> Animations;
    	
        glm::mat4 InterpolatePos(glm::vec3 out, float timeticks, const aiNodeAnim* animnode);
    	u_int FindPos(float timeticks, const aiNodeAnim* animnode);
    	glm::mat4 InterpolateRot(glm::quat out, float timeticks, const aiNodeAnim* animnode);
    	u_int FindRot(float timeticks, const aiNodeAnim* animnode);
    	glm::mat4 InterpolateScale(glm::vec3 out, float timeticks, const aiNodeAnim* animnode);
    	u_int FindScale(float timeticks, const aiNodeAnim* animnode);
    
    	    
    	void reload(int id) {
    		/*animations[id].selectedpath = animations[id].animpaths[animations[id].pathindex];*/
    		/*scene[animations[id].sceneind] = importer[animations[id].sceneind].ReadFile(animations[id].selectedpath, IMPORT_PROPS);*/
    	}
     
    	const aiNodeAnim* FindAnim(const aiAnimation* anim, const std::string nodename);
    	void ReadNodeHierarchy(Gfx::ModelInfo* model, int animid, const aiNode* node, float timetick, const glm::mat4 parenttransform);
        std::vector<glm::mat4> GetBonesTransform(Gfx::ModelInfo* model, int animid, float time);
  
  };
}
