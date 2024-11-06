#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/core/deletor.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>

#define IMPORT_PROPS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)

namespace Gfx
{
    struct ModelInfo {
        std::string texturename;
        std::vector<MeshInfo*> Meshes;
        std::vector<int> MeshBase;
    };
    typedef std::unordered_map<std::string, ModelInfo> ModelsMap;

    class Model : public Utils::Singleton<Model>
    {
        public:
            Model() {}
            ~Model() {
                for (auto& model : Models) {
                    for (auto& mesh : model.second.Meshes) {
                        delete mesh;
                    }
                }
            }
            void LoadModels();
            void LoadModel(const std::string& path);
            ModelInfo* GetModel(const std::string& path);

        private:
            MeshInfo ProcessMesh(const aiScene *scene, aiMesh* aimesh);
            void ProcessNode(const std::string& path, aiNode *node, const aiScene *scene);
            
            ModelsMap Models;

    };
}
