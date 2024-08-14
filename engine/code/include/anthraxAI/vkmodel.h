#pragma once

#include "anthraxAI/vkmesh.h"
#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"
#include "anthraxAI/vkbuffer.h"
#include "anthraxAI/vktexture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <map>

class ModelBuilder : public MeshBuilder 
{
public:
    ModelBuilder() {}
    ~ModelBuilder() { 
        for (auto& it : models ) {
            if (!it.second.meshes.empty()) {
                for (int i = 0 ; i < it.second.meshes.size(); i++) {
                    delete it.second.meshes[i];
                }
            }
        }
        
    }

    int index = 0;
    void init(DeletionQueue* del)                   { deletor = del; }
	Model* getmodel(int id);

    void loadmodel(std::string path, int id);
    Mesh processmesh(const aiScene *scene, aiMesh* aimesh,int id);
    void processnode(aiNode *node, const aiScene *scene, int id);
    void processbones(std::vector<Vertex>& vert, int id, aiMesh* aimesh);
    int  getboneid(int modelind, const aiBone* bone);

    void setvertexbonedata(Vertex& vert, int id, float weight);

    void updatedescriptors(RenderBuilder& renderer,  DescriptorBuilder* desc);

private:
	DeletionQueue*	deletor;
    int totalvert = 0;

    int bonecounter = 0;
    std::unordered_map
	<int, Model> models;
};