#include "anthraxAI/core/animator.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/engine.h"

std::vector<glm::mat4> Core::AnimatorBase::Update(Gfx::RenderObject& object)
{
    if (Animations.empty()) {
        return {glm::mat4(1.0f)};
    }

    float timesec = (float)((double)Engine::GetInstance()->GetTime() - (double)Utils::Debug::GetInstance()->AnimStartMs) / 1000.0f;
	
    std::vector<glm::mat4> vec =  GetBonesTransform(object.Model, object.ID, timesec);
    return vec;
    //for (Core::AnimationData& data : Animations) {
  }

const aiNodeAnim* Core::AnimatorBase::FindAnim(const aiAnimation* anim, const std::string nodename)
{
    for (int i = 0; i< anim->mNumChannels; i++) {
        const aiNodeAnim* node = anim->mChannels[i];

        if (std::string(node->mNodeName.data) == nodename) {
            return node;
        }
    }
    return nullptr;
}

void Core::AnimatorBase::Init() 
{
    Core::Scene* scene = Core::Scene::GetInstance();
    for (auto& it : scene->GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetModelName().empty() || !info->HasAnimations()) continue;
            AnimationData& data =  Animations[info->GetID()];

            data.Paths.reserve(info->GetAnimations().size());
            for (const std::string& animpath : info->GetAnimations()) {
                data.Paths.push_back("./models/" + animpath);
            }
            data.CurrentPath = data.Paths[0];
            data.PathIndex = 0;
            data.SceneInd = 0;// Animations.size() - 1;
            if (!Scenes[0]) {
                Scenes[data.SceneInd] = Importer[data.SceneInd].ReadFile(data.CurrentPath, IMPORT_PROPS);
            }
        }
    }
    GlobalInverse = glm::inverse(glm::mat4(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0));
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    GlobalInverse *= rot;
}

void Core::AnimatorBase::ReadNodeHierarchy(Gfx::ModelInfo* model, int animid, const aiNode* node, float timetick, const glm::mat4 parenttransform)
{ 
    std::string nodename = node->mName.C_Str();
    const aiAnimation* anim = Scenes[Animations[animid].SceneInd]->mAnimations[0];
    // printf("%d|%s\n\n",animid, std::string(anim->mName.C_Str()).c_str());
    const aiNodeAnim* nodeanim = FindAnim(anim, nodename);

   glm::mat4 nodetransf = Gfx::mat2glm(node->mTransformation);

    if (nodeanim) {
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

    for (int i = 0; i < node->mNumChildren; i++) {
        ReadNodeHierarchy(model, animid, node->mChildren[i], timetick, globaltransf);
    }
}

std::vector<glm::mat4> Core::AnimatorBase::GetBonesTransform(Gfx::ModelInfo* model, int animid, float time)
{
    Core::AnimationData& data = Animations[animid];

    float tickespersec = (float)(Scenes[data.SceneInd]->mAnimations[0]->mTicksPerSecond != 0 ? Scenes[data.SceneInd]->mAnimations[0]->mTicksPerSecond : 25.0f);
    
    float timeinticks = time * tickespersec;
    float timeticks =  fmod(timeinticks, (float)Scenes[data.SceneInd]->mAnimations[0]->mDuration);

    ReadNodeHierarchy(model, animid, Scenes[data.SceneInd]->mRootNode, timeticks, glm::mat4(1.0));
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

glm::mat4 Core::AnimatorBase::InterpolatePos(glm::vec3 out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumPositionKeys == 1) {
        out = Gfx::vec2glm(animnode->mPositionKeys[0].mValue);
        return glm::translate(glm::mat4(1.0f), out);
    }

    u_int PositionIndex = FindPos(timeticks, animnode);
    u_int NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < animnode->mNumPositionKeys);
    float t1 = (float)animnode->mPositionKeys[PositionIndex].mTime;
    float t2 = (float)animnode->mPositionKeys[NextPositionIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::vec3& Start = Gfx::vec2glm(animnode->mPositionKeys[PositionIndex].mValue);
    const glm::vec3& End = Gfx::vec2glm(animnode->mPositionKeys[NextPositionIndex].mValue);
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);
    return glm::translate(glm::mat4(1.0f), out);
}

u_int Core::AnimatorBase::FindPos(float timeticks, const aiNodeAnim* animnode)
{
    assert(animnode->mNumPositionKeys > 0);

    for (u_int i = 0 ; i < animnode->mNumPositionKeys - 1 ; i++) {
        float t = (float)animnode->mPositionKeys[i + 1].mTime;
        if (timeticks < t) {
            return i;
        }
    }
    return 0;
}

glm::mat4 Core::AnimatorBase::InterpolateRot(glm::quat out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumRotationKeys == 1) {
        return glm::toMat4(static_cast<glm::quat>(Gfx::quat2glm(animnode->mRotationKeys[0].mValue)));
    }

    u_int RotationIndex = FindRot(timeticks, animnode);
    u_int NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < animnode->mNumRotationKeys);
    float t1 = (float)animnode->mRotationKeys[RotationIndex].mTime;
    float t2 = (float)animnode->mRotationKeys[NextRotationIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::quat& StartRotationQ = Gfx::quat2glm(animnode->mRotationKeys[RotationIndex].mValue);
    const glm::quat& EndRotationQ = Gfx::quat2glm(animnode->mRotationKeys[NextRotationIndex].mValue);
    glm::quat finrot = glm::slerp(static_cast<glm::quat>(StartRotationQ), static_cast<glm::quat>(EndRotationQ), Factor);
    finrot = glm::normalize(finrot);
    return glm::toMat4(finrot);
}

u_int Core::AnimatorBase::FindRot(float timeticks, const aiNodeAnim* animnode)
{
    assert(animnode->mNumRotationKeys > 0);

    for (u_int i = 0 ; i < animnode->mNumRotationKeys - 1 ; i++) {
        float t = (float)animnode->mRotationKeys[i + 1].mTime;
        if (timeticks < t) {
            return i;
        }
    }

    return 0;
}

glm::mat4 Core::AnimatorBase::InterpolateScale(glm::vec3 out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumScalingKeys == 1) {
        out = Gfx::vec2glm(animnode->mScalingKeys[0].mValue);
        return glm::scale(glm::mat4(1.0f), out);
    }

    u_int ScalingIndex = FindScale(timeticks, animnode);
    u_int NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < animnode->mNumScalingKeys);
    float t1 = (float)animnode->mScalingKeys[ScalingIndex].mTime;
    float t2 = (float)animnode->mScalingKeys[NextScalingIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - (float)t1) / DeltaTime;
    const glm::vec3& Start = Gfx::vec2glm(animnode->mScalingKeys[ScalingIndex].mValue);
    const glm::vec3& End = Gfx::vec2glm(animnode->mScalingKeys[NextScalingIndex].mValue);
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);//Start + Factor * Delta;
    return glm::scale(glm::mat4(1.0f), out);
}

u_int Core::AnimatorBase::FindScale(float timeticks, const aiNodeAnim* animnode)
{
     assert(animnode->mNumScalingKeys > 0);

    for (u_int i = 0 ; i < animnode->mNumScalingKeys - 1 ; i++) {
        float t = (float)animnode->mScalingKeys[i + 1].mTime;
        if (timeticks < t) {
            return i;
        }
    }

    return 0;   
}
