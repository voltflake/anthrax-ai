#include "anthraxAI/animator.h"

const aiNodeAnim* Animator::findanim(const aiAnimation* anim, const std::string nodename)
{
    for (int i = 0; i< anim->mNumChannels; i++) {
        const aiNodeAnim* node = anim->mChannels[i];

        if (std::string(node->mNodeName.data) == nodename) {
            return node;
        }
    }
    return nullptr;
}

u_int Animator::findscale(float timeticks, const aiNodeAnim* animnode)
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

glm::mat4 Animator::interpolatescale(glm::vec3 out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumScalingKeys == 1) {
        out = vec2glm(animnode->mScalingKeys[0].mValue);
        return glm::scale(glm::mat4(1.0f), out);
    }

    u_int ScalingIndex = findscale(timeticks, animnode);
    u_int NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < animnode->mNumScalingKeys);
    float t1 = (float)animnode->mScalingKeys[ScalingIndex].mTime;
    float t2 = (float)animnode->mScalingKeys[NextScalingIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - (float)t1) / DeltaTime;
    const glm::vec3& Start = vec2glm(animnode->mScalingKeys[ScalingIndex].mValue);
    const glm::vec3& End   = vec2glm(animnode->mScalingKeys[NextScalingIndex].mValue);
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);//Start + Factor * Delta;
    return glm::scale(glm::mat4(1.0f), out);
}

u_int Animator::findrot(float timeticks, const aiNodeAnim* animnode)
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


glm::mat4 Animator::interpolaterot(glm::quat out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumRotationKeys == 1) {
        return glm::toMat4(static_cast<glm::quat>(quat2glm(animnode->mRotationKeys[0].mValue)));
    }

    u_int RotationIndex = findrot(timeticks, animnode);
    u_int NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < animnode->mNumRotationKeys);
    float t1 = (float)animnode->mRotationKeys[RotationIndex].mTime;
    float t2 = (float)animnode->mRotationKeys[NextRotationIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::quat& StartRotationQ = quat2glm(animnode->mRotationKeys[RotationIndex].mValue);
    const glm::quat& EndRotationQ   = quat2glm(animnode->mRotationKeys[NextRotationIndex].mValue);
    glm::quat finrot = glm::slerp(static_cast<glm::quat>(StartRotationQ), static_cast<glm::quat>(EndRotationQ), Factor);
    finrot = glm::normalize(finrot);
    return glm::toMat4(finrot);
}

u_int Animator::findpos(float timeticks, const aiNodeAnim* animnode)
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

glm::mat4 Animator::interpolatepos(glm::vec3 out, float timeticks, const aiNodeAnim* animnode)
{
    if (animnode->mNumPositionKeys == 1) {
        out = vec2glm(animnode->mPositionKeys[0].mValue);
        return glm::translate(glm::mat4(1.0f), out);
    }

    u_int PositionIndex = findpos(timeticks, animnode);
    u_int NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < animnode->mNumPositionKeys);
    float t1 = (float)animnode->mPositionKeys[PositionIndex].mTime;
    float t2 = (float)animnode->mPositionKeys[NextPositionIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor = (timeticks - t1) / DeltaTime;
    const glm::vec3& Start = vec2glm(animnode->mPositionKeys[PositionIndex].mValue);
    const glm::vec3& End = vec2glm(animnode->mPositionKeys[NextPositionIndex].mValue);
    glm::vec3 Delta = End - Start;
    out = glm::mix(Start, End, Factor);
    return glm::translate(glm::mat4(1.0f), out);
}

void Animator::readnodehierarchy(Model* model, int animid, const aiNode* node, float timetick, const glm::mat4 parenttransform)
{
    std::string nodename = node->mName.C_Str();
    const aiAnimation* anim = scene[animations[animid].sceneind]->mAnimations[0];
    // printf("%d|%s\n\n",animid, std::string(anim->mName.C_Str()).c_str());
    const aiNodeAnim* nodeanim = findanim(anim, nodename);

   glm::mat4 nodetransf = mat2glm(node->mTransformation);

    if (nodeanim) {
        glm::vec3 scaling;
        glm::mat4 scm = interpolatescale(scaling, timetick, nodeanim);
   
        glm::quat rot;
        glm::mat4 rotm = interpolaterot(rot, timetick, nodeanim);

        glm::vec3 Translation;
        glm::mat4 transl = interpolatepos(Translation, timetick, nodeanim);

        nodetransf = transl * rotm * scm; 
    }
    glm::mat4 globaltransf = parenttransform * nodetransf;

    if (model->bonamap.find(nodename) != model->bonamap.end()) {
        int boneind = model->bonamap[nodename];
        model->boneinfo[boneind].fintransform = globinverse * globaltransf * model->boneinfo[boneind].offset;
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        readnodehierarchy(model, animid, node->mChildren[i],timetick, globaltransf);
    }
}

std::vector<glm::mat4> Animator::getbonestransform(Model* model, int animid, float time)
{
    float tickespersec = (float)(scene[animations[animid].sceneind]->mAnimations[0]->mTicksPerSecond != 0 ? scene[animations[animid].sceneind]->mAnimations[0]->mTicksPerSecond : 25.0f);
    float timeinticks = time * tickespersec;
    float timeticks =  fmod(timeinticks, (float)scene[animations[animid].sceneind]->mAnimations[0]->mDuration);

    readnodehierarchy(model, animid, scene[animations[animid].sceneind]->mRootNode, timeticks, glm::mat4(1.0));
    std::vector<glm::mat4> vec;
    vec.resize(model->boneinfo.size(), glm::mat4(1.0));
    model->fintransforms.resize(model->boneinfo.size());

    for (int i = 0; i < model->boneinfo.size(); i++) {
        vec[i] = (model->boneinfo[i].fintransform);
        model->fintransforms[i] = model->boneinfo[i].fintransform;

    //    printf("\n\tmodel id: %d", animid);
    //    printf("\n|%f||%f||%f|\n|%f||%f||%f|\n", vec[i][0][0], vec[i][0][1],vec[i][0][2],vec[i][1][0],vec[i][2][0],vec[i][3][0]);
    }
    // printf("-----------------------------\n");
    return  vec;
}

