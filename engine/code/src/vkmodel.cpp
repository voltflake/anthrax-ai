#include "anthraxAI/vkmodel.h"

Model* ModelBuilder::getmodel(int id)
{
	auto it = models.find(id);
	if (it == models.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}

void ModelBuilder::updatedescriptors(RenderBuilder& renderer,  DescriptorBuilder* desc)
{
    BufferBuilder buffer;
    for (auto& model : models)
	{
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = desc->getdescriptorpool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &(desc->getstoragelayout());

        VK_ASSERT(vkAllocateDescriptorSets(renderer.getdevice()->getlogicaldevice(), &allocInfo, &model.second.descritor), "failed to allocate descriptor sets!");


        buffer.createbuffer(renderer, model.second.storagebuffer, BONE_ARRAY_SIZE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            
        VkDescriptorBufferInfo storageinfo;
        storageinfo.buffer = model.second.storagebuffer.buffer;
        storageinfo.offset = 0;
        storageinfo.range = BONE_ARRAY_SIZE;
        
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;

        write.dstBinding = 0;
        write.dstSet = model.second.descritor;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.pBufferInfo = &storageinfo;

        vkUpdateDescriptorSets(renderer.getdevice()->getlogicaldevice(), 1, &write, 0, nullptr);
	}
}

void ModelBuilder::setvertexbonedata(Vertex& vert, int id, float weight)
{
    for (int i = 0; i < BONE_INFLUENCE; i++) {
        if ( vert.boneID[i] < 0) {
            vert.boneID[i] = id;
            vert.weights[i] = weight;
            break;
        }
    }
}

Mesh ModelBuilder::processmesh(const aiScene *scene, aiMesh* mesh, int id)
{
    Mesh model;

    for(int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
       
        setvertexbonedefaultdata(vertex);
		glm::vec3 data; 
		data.x = mesh->mVertices[i].x;
		data.y = mesh->mVertices[i].y;
		data.z = mesh->mVertices[i].z; 
		vertex.position = data;

		data.x = mesh->mNormals[i].x;
		data.y = mesh->mNormals[i].y;
		data.z = mesh->mNormals[i].z;
		vertex.normal = data;
		if(mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x; 
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;
		}
		else {
			vertex.uv = glm::vec2(0);
		}
		vertex.color = {1.0f, 1.0f, 1.0f};
        vertex.color = vertex.normal;

		model.vertices.push_back(vertex);

    }

	for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++) {
			model.aiindices.push_back(face.mIndices[j]);
		}
	}

  

    return model;
}

int  ModelBuilder::getboneid(int modelind, const aiBone* bone)
{
    int boneid = 0;
    std::string bonename(bone->mName.C_Str());

    if (models[modelind].bonamap.find(bonename) == models[modelind].bonamap.end()) {
        boneid = models[modelind].bonamap.size();
        models[modelind].bonamap[bonename] = boneid;
    }
    else {
        boneid = models[modelind].bonamap[bonename];
    }
    return boneid;
}


void ModelBuilder::processbones(std::vector<Vertex>& vert, int id, aiMesh *mesh)
{
    for(unsigned int j = 0; j < mesh->mNumBones; j++) {

        int boneID = -1;
        std::string bonename = mesh->mBones[j]->mName.C_Str();
        if (models[id].bonamap.find(bonename) == models[id].bonamap.end()) {
            BoneInfo bone;
            models[id].boneinfo.push_back(bone);
            boneID = bonecounter;
            bonecounter++;
        }
        else {
            boneID = models[id].bonamap[bonename];
        }

        models[id].bonamap[bonename] = boneID;
        models[id].boneinfo[boneID] = mat2glm(mesh->mBones[j]->mOffsetMatrix);
        ASSERT(boneID == -1, "bone loading error\n");
        aiVertexWeight* weights = mesh->mBones[j]->mWeights;
        int numweights = mesh->mBones[j]->mNumWeights;

        for (int weightind = 0; weightind < numweights; ++weightind) {
            int vertexID = weights[weightind].mVertexId;
            float weight = weights[weightind].mWeight;
            ASSERT(!(vertexID <= vert.size()), "vertex weight loading error\n");
            setvertexbonedata(vert[vertexID], boneID, weight);
        }
    }
}



void ModelBuilder::processnode(aiNode *node, const aiScene *scene, int id)
{
    models[id].meshbase.resize(scene->mNumMeshes);

    for(int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[i];

        Mesh* tmpmesh = new Mesh;
        *tmpmesh = processmesh(scene, mesh, id);
       
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            aiString texturepath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texturepath);
            models[id].texturepath = texturepath.C_Str();

            printf("------ [%d] assimp model texture path: %s \n\n", id, models[id].texturepath.c_str());
        }

        models[id].meshbase[i] = totalvert;
        totalvert += mesh->mNumVertices;
        models[id].vert2bones.resize(totalvert);

        if (mesh->HasBones()) {
            processbones(tmpmesh->vertices, id, mesh);        
        }
        models[id].meshes.push_back(tmpmesh);
    }
}

void ModelBuilder::loadmodel(std::string path, int id)
{
    Assimp::Importer importer;
    const aiScene* scene = NULL;

    scene = importer.ReadFile(path, IMPORT_PROPS);
    ASSERT(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode, (importer.GetErrorString())); 

    totalvert = 0;
    bonecounter = 0;
    processnode(scene->mRootNode, scene, id);
   
    importer.FreeScene();
}
