#include "anthraxAI/core/animator.h"
#include "anthraxAI/gfx/model.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/engine.h"
#include "assimp/anim.h"
#include "assimp/scene.h"

std::vector<glm::mat4> Core::AnimatorBase::Update(Gfx::RenderObject& object)
{
    if (Animations.empty()) {
        return {glm::mat4(1.0f)};
    }

    float timesec = (float)((double)Engine::GetInstance()->GetTime() - (double)Utils::Debug::GetInstance()->AnimStartMs) / 1000.0f;
	
    std::vector<glm::mat4> vec =  GetBonesTransform(object.Model, object.ID, timesec);
    return vec;
}

const Core::NodeAnim& Core::AnimatorBase::FindAnim(const aiSceneInfo& scene, const std::string nodename)
{
    for (int i = 0; i < scene.AnimNodes.size(); i++) {
        //const aiNodeAnim* node = anim->mChannels[i];
        const NodeAnim& node = scene.AnimNodes[i];
        if (node.NodeName == nodename) {
            return node;
        }
    }
    return EMPTY_ANIM_NODE;
}

void Core::AnimatorBase::GetNodeChildren(const aiNode* node, Core::NodeRoots& info)
{
    info.Name = node->mName.C_Str();
    info.Tranform = Gfx::mat2glm(node->mTransformation);
    info.ChildrenNum = node->mNumChildren;
    info.Children.reserve(info.ChildrenNum);
    for (int i = 0; i < node->mNumChildren; i++) {
        NodeRoots nodetmp;
        GetNodeChildren(node->mChildren[i], nodetmp);
        info.Children.emplace_back(nodetmp);
    }    
}

Core::aiSceneInfo Core::AnimatorBase::ConvertAi(const aiScene* scene)
{
    Core::aiSceneInfo info;

    info.TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond;
    info.Duration = scene->mAnimations[0]->mDuration;

    
    Core::NodeRoots root;
    GetNodeChildren(scene->mRootNode, root);
    info.RootNode = root;

    info.AnimNodes.resize(scene->mAnimations[0]->mNumChannels);
    
    for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++) {
        const aiNodeAnim* node = scene->mAnimations[0]->mChannels[i];

        NodeAnim anim;
        anim.NodeName = node->mNodeName.data;// C_Str();
        anim.NumPositionsKeys = node->mNumPositionKeys;
        anim.NumRotationKeys = node->mNumRotationKeys;
        anim.NumScalingKeys = node->mNumScalingKeys;
        
        anim.PositionKeys.reserve(anim.NumPositionsKeys);
        anim.PositionTime.reserve(anim.NumPositionsKeys);
        for (int posi = 0; posi < node->mNumPositionKeys; posi++) {
            anim.PositionKeys.emplace_back(Gfx::vec2glm(node->mPositionKeys[posi].mValue));
            anim.PositionTime.emplace_back(node->mPositionKeys[posi].mTime);
        }
        anim.RotationKeys.reserve(anim.NumRotationKeys);
        anim.RotationTime.reserve(anim.NumRotationKeys);
        for (int posi = 0; posi < node->mNumRotationKeys; posi++) {
            anim.RotationKeys.emplace_back(Gfx::quat2glm(node->mRotationKeys[posi].mValue));
            anim.RotationTime.emplace_back(node->mRotationKeys[posi].mTime);
        }
        anim.ScalingKeys.reserve(anim.NumScalingKeys);
        anim.ScalingTime.reserve(anim.NumScalingKeys);
        for (int posi = 0; posi < node->mNumScalingKeys; posi++) {
            anim.ScalingKeys.emplace_back(Gfx::vec2glm(node->mScalingKeys[posi].mValue));
            anim.ScalingTime.emplace_back(node->mScalingKeys[posi].mTime);
        }
        info.AnimNodes.emplace_back(anim);
    }

    return info;
}

void Core::AnimatorBase::Init() 
{
    Core::Scene* scene = Core::Scene::GetInstance();
    for (auto& it : scene->GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetModelName().empty() || !info->HasAnimations()) continue;
            AnimationData& data =  Animations[info->GetID()];

            data.Paths.reserve(info->GetAnimations().size());
            data.SceneInd = 0;
            for (const std::string& animpath : info->GetAnimations()) {
                data.Paths.push_back("./models/" + animpath);
                
                auto it = std::find(AnimationMap.begin(), AnimationMap.end(), "./models/" + animpath);

                std::size_t index = std::distance(std::begin(AnimationMap), it);
                if (it == AnimationMap.end()) {
                    const aiScene* scene = Importer.ReadFile("./models/" + animpath, IMPORT_PROPS);
                    aiSceneInfo info = ConvertAi(scene);
                    Scenes.push_back(info);
                    AnimationMap.push_back(std::string("./models/" + animpath));
                    data.SceneInd = AnimationMap.size() - 1;
                    Importer.FreeScene();
                }
            }
            data.CurrentPath = data.Paths[0];
            data.PathIndex = 0;
        }
    }
    GlobalInverse = glm::inverse(glm::mat4(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0));
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    GlobalInverse *= rot;
}

void Core::AnimatorBase::ReadNodeHierarchy(Gfx::ModelInfo* model, int animid, const Core::aiSceneInfo& scene, const Core::NodeRoots& node, float timetick, const glm::mat4 parenttransform)
{ 
    std::string nodename = node.Name;

    const NodeAnim& nodeanim = FindAnim(scene, nodename);

   glm::mat4 nodetransf = node.Tranform;

    if (!nodeanim.NodeName.empty()) {
        glm::vec3 scaling;
        glm::mat4 scm = InterpolateScale(scaling, timetick, nodeanim);
   
        glm::quat rot;
        glm::mat4 rotm = InterpolateRot(rot, timetick, nodeanim);

        glm::vec3 Translation;
        glm::mat4 transl = InterpolatePos(Translation, timetick, nodeanim);

        nodetransf = transl * rotm * scm; 
    }
    glm::mat4 globaltransf = parenttransform * nodetransf;

    if (model->Bones.BoneMap.find(nodename) != model->Bones.BoneMap.end()) {
        int boneind = model->Bones.BoneMap[nodename];
        model->Bones.Info[boneind].FinTransform = GlobalInverse * globaltransf * model->Bones.Info[boneind].Offset;
    }

    for (int i = 0; i < node.ChildrenNum; i++) {
        ReadNodeHierarchy(model, animid, scene, node.Children[i], timetick, globaltransf);
    }
}

std::vector<glm::mat4> Core::AnimatorBase::GetBonesTransform(Gfx::ModelInfo* model, int animid, float time)
{
    Core::AnimationData& data = Animations[animid];
    
    Core::aiSceneInfo& scene = Scenes[data.SceneInd];

    float tickespersec = (float)(scene.TicksPerSecond != 0 ? scene.TicksPerSecond : 25.0f);
    
    float timeinticks = time * tickespersec;
    float timeticks =  fmod(timeinticks, (float)scene.Duration);

    ReadNodeHierarchy(model, animid, scene, scene.RootNode, timeticks, glm::mat4(1.0));
    std::vector<glm::mat4> vec;
    vec.resize(model->Bones.Info.size(), glm::mat4(1.0));
    model->Bones.FinTransform.reserve(model->Bones.Info.size());

    for (int i = 0; i < model->Bones.Info.size(); i++) {
        vec[i] = (model->Bones.Info[i].FinTransform);
        model->Bones.FinTransform[i] = model->Bones.Info[i].FinTransform;

    //    printf("\n\tmodel id: %d", animid);
    //    printf("\n|%f||%f||%f|\n|%f||%f||%f|\n", vec[i][0][0], vec[i][0][1],vec[i][0][2],vec[i][1][0],vec[i][2][0],vec[i][3][0]);
    }
    // printf("-----------------------------\n");
    return  vec;
}

glm::mat4 Core::AnimatorBase::InterpolatePos(glm::vec3 out, float timeticks, const NodeAnim& animnode)
{
    if (animnode.NumPositionsKeys == 1) {
        out = (animnode.PositionKeys[0]);
        return glm::translate(glm::mat4(1.0f), out);
    }

    u_int PositionIndex = FindPos(timeticks, animnode);
    u_int NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < animnode.NumPositionsKeys);
    float t1 = (float)animnode.PositionTime[PositionIndex];
    float t2 = (float)animnode.PositionTime[NextPositionIndex];
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::vec3& Start = animnode.PositionKeys[PositionIndex];
    const glm::vec3& End = animnode.PositionKeys[NextPositionIndex];
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);
    return glm::translate(glm::mat4(1.0f), out);
}

u_int Core::AnimatorBase::FindPos(float timeticks, const NodeAnim& animnode)
{
    assert(animnode.NumPositionsKeys > 0);

    for (u_int i = 0 ; i < animnode.NumPositionsKeys - 1 ; i++) {
        float t = (float)animnode.PositionTime[i + 1];
        if (timeticks < t) {
            return i;
        }
    }
    return 0;
}

glm::mat4 Core::AnimatorBase::InterpolateRot(glm::quat out, float timeticks, const NodeAnim& animnode)
{
    if (animnode.NumRotationKeys == 1) {
        return glm::toMat4(static_cast<glm::quat>((animnode.RotationKeys[0])));
    }

    u_int RotationIndex = FindRot(timeticks, animnode);
    u_int NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < animnode.NumRotationKeys);
    float t1 = (float)animnode.RotationTime[RotationIndex];
    float t2 = (float)animnode.RotationTime[NextRotationIndex];
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::quat& StartRotationQ = animnode.RotationKeys[RotationIndex];
    const glm::quat& EndRotationQ = animnode.RotationKeys[NextRotationIndex];
    glm::quat finrot = glm::slerp(static_cast<glm::quat>(StartRotationQ), static_cast<glm::quat>(EndRotationQ), Factor);
    finrot = glm::normalize(finrot);
    return glm::toMat4(finrot);
}

u_int Core::AnimatorBase::FindRot(float timeticks, const NodeAnim& animnode)
{
    assert(animnode.NumRotationKeys > 0);

    for (u_int i = 0 ; i < animnode.NumRotationKeys - 1 ; i++) {
        float t = (float)animnode.RotationTime[i + 1];
        if (timeticks < t) {
            return i;
        }
    }

    return 0;
}

glm::mat4 Core::AnimatorBase::InterpolateScale(glm::vec3 out, float timeticks, const NodeAnim& animnode)
{
    if (animnode.NumScalingKeys == 1) {
        out = animnode.ScalingKeys[0];
        return glm::scale(glm::mat4(1.0f), out);
    }

    u_int ScalingIndex = FindScale(timeticks, animnode);
    u_int NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < animnode.NumScalingKeys);
    float t1 = (float)animnode.ScalingTime[ScalingIndex];
    float t2 = (float)animnode.ScalingTime[NextScalingIndex];
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - (float)t1) / DeltaTime;
    const glm::vec3& Start = animnode.ScalingKeys[ScalingIndex];
    const glm::vec3& End = animnode.ScalingKeys[NextScalingIndex];
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);//Start + Factor * Delta;
    return glm::scale(glm::mat4(1.0f), out);
}

u_int Core::AnimatorBase::FindScale(float timeticks, const NodeAnim& animnode)
{
     assert(animnode.NumScalingKeys > 0);

    for (u_int i = 0 ; i < animnode.NumScalingKeys - 1 ; i++) {
        float t = (float)animnode.ScalingTime[i + 1];
        if (timeticks < t) {
            return i;
        }
    }

    return 0;   
}
