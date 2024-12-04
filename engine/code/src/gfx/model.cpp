#include "anthraxAI/gfx/model.h"
#include "anthraxAI/core/scene.h"
#include "anthraxAI/gfx/vkbase.h"

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
   VkDebugUtilsObjectNameInfoEXT info;
	info.pNext = nullptr;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectHandle = reinterpret_cast<uint64_t>(meshinfo->IndexBuffer.DeviceMemory);
	info.objectType = VK_OBJECT_TYPE_DEVICE_MEMORY;;
	info.pObjectName = std::string("moedl index buffer" + meshinfo->Path).c_str();
	Gfx::Vulkan::GetInstance()->SetDebugName(info);

	info.pNext = nullptr;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectHandle = reinterpret_cast<uint64_t>(meshinfo->VertexBuffer.DeviceMemory);
	info.objectType = VK_OBJECT_TYPE_DEVICE_MEMORY;;
	info.pObjectName = std::string("moedl vertex buffer" + meshinfo->Path).c_str();
	Gfx::Vulkan::GetInstance()->SetDebugName(info);
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

void Gfx::Model::CleanAll()
{
    for (auto& model : Models) {
        for (auto& mesh : model.second.Meshes) {
            mesh->Clean();
            delete mesh;
        }
        model.second.Meshes.clear();
    }
    Models.clear();
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


