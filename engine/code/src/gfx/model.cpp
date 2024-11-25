#include "anthraxAI/gfx/model.h"
#include "anthraxAI/core/scene.h"
void Gfx::Model::ProcessNode(const std::string& path, aiNode *node, const aiScene *scene)
{
    Models[path].MeshBase.resize(scene->mNumMeshes);
    Models[path].texturename = path;
    int meshsize = scene->mNumMeshes;
    Models[path].Meshes.reserve(meshsize);
    for (int i = 0; i < meshsize; i++) {

        aiMesh* aimesh = scene->mMeshes[i];

        Gfx::MeshInfo* meshinfo = new Gfx::MeshInfo;
        Gfx::Mesh::GetInstance()->CreateMesh(aimesh, meshinfo);

        Models[path].Meshes.push_back(meshinfo);
         
        // printf("-----!!!!------\n\n mesh \n\n ----!!!!--");
        //   if (mesh->mMaterialIndex >= 0) {
            // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // aiString texturepath;
            // // material->GetTexture(aiTextureType_DIFFUSE, 0, &texturepath);
            // // models[id].texturepath = texturepath.C_Str();

            // printf("------ [%d] assimp model texture path: %s \n\n", id, models[id].texturepath.c_str());
        // }

        // models[id].meshbase[i] = totalvert;
        // // totalvert += mesh->mNumVertices;
        // // models[id].vert2bones.resize(totalvert);

        // if (mesh->HasBones()) {
            // processbones(tmpmesh->vertices, id, mesh);        
        // }
    }
}

void Gfx::Model::LoadModels()
{
    std::string path = "./models/";
    Core::Scene* scene = Core::Scene::GetInstance();
    for (auto& it : scene->GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetModelName().empty()) continue;

            LoadModel(path + info->GetModelName());
        }
    }
    LoadModel(path + "axis.obj");
}

void Gfx::Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = nullptr;

    scene = importer.ReadFile(path, IMPORT_PROPS);
    ASSERT(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode, (importer.GetErrorString())); 

    ProcessNode(path, scene->mRootNode, scene);

    importer.FreeScene();
}

Gfx::ModelInfo* Gfx::Model::GetModel(const std::string& path)
{
	auto it = Models.find("./models/" + path);
	if (it == Models.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}

Gfx::MeshInfo Gfx::Model::ProcessMesh(const aiScene *scene, aiMesh* aimesh)
{

}

