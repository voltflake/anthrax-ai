#pragma once

#include "anthraxAI/gfx/model.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "glm/fwd.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

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

    struct NodeRoots {
        std::string Name;
        glm::mat4 Transform;

        int ChildrenNum;
        std::vector<NodeRoots> Children;
    };
    struct NodeAnim {
        std::string NodeName;
        uint32_t NumPositionsKeys;
        std::vector<glm::vec3> PositionKeys;
        std::vector<float> PositionTime;
        uint32_t NumRotationKeys;
        std::vector<glm::quat> RotationKeys;
        std::vector<float> RotationTime;
        uint32_t NumScalingKeys;
        std::vector<glm::vec3> ScalingKeys;
        std::vector<float> ScalingTime;

        NodeAnim() {}
    };
    struct aiSceneInfo {
        float TicksPerSecond;
        float Duration;
        NodeRoots RootNode;

        std::vector<NodeAnim> AnimNodes;

    };
  class AnimatorBase {

    public:
        AnimatorBase() {};
  	    void Init();
        void Update(Gfx::RenderObject& object);

        bool HasAnimation(int id) {
    		return Animations.find(id) != Animations.end();
    	}

    void Reload(int id, const std::string& name) {
            Animations[id].CurrentPath = name;
            auto it = std::find(AnimationMap.begin(), AnimationMap.end(), "./models/" + name);
            std::size_t index = std::distance(AnimationMap.begin(), it);
            Animations[id].SceneInd = index;
            printf("INDEX %d\n", index);
    	}


    private:
    	Assimp::Importer Importer;
    	//const aiScene* Scenes[10] = {NULL};
        std::vector<aiSceneInfo> Scenes;

        glm::mat4 GlobalInverse;
    	std::map<int, AnimationData> Animations;
        std::vector<std::string> AnimationMap;

        aiSceneInfo ConvertAi(const aiScene* scene);

        glm::mat4 InterpolatePos(glm::vec3 out, float timeticks, const NodeAnim& animnode);
    	u_int FindPos(float timeticks, const NodeAnim& animnode);
    	glm::mat4 InterpolateRot(glm::quat out, float timeticks, const NodeAnim& animnode);
    	u_int FindRot(float timeticks, const NodeAnim& animnode);
    	glm::mat4 InterpolateScale(glm::vec3 out, float timeticks, const NodeAnim& nodeanim );
    	u_int FindScale(float timeticks, const NodeAnim& animnode);


    	    	const Core::NodeAnim& FindAnim(const aiSceneInfo& scene, const std::string nodename);
    	void ReadNodeHierarchy(Gfx::ModelInfo* model, int animid, const Core::aiSceneInfo& scene, const NodeRoots& node, float timetick, const glm::mat4 parenttransform);
        void GetBonesTransform(Gfx::ModelInfo* model, int animid, float time);

        void GetNodeChildren(const aiNode* node, Core::NodeRoots& info);

        Core::NodeAnim EMPTY_ANIM_NODE;
  };
}
