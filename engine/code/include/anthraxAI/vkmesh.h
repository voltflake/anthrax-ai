#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"
#include "anthraxAI/vkbuffer.h"
#include "anthraxAI/vktexture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>

#define IMPORT_PROPS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
struct VertexBoneData {
	int boneId[BONE_INFLUENCE]  ={0};
	float weight[BONE_INFLUENCE]  ={0.0f};

	void add(int id, float w) {
		for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(boneId); i++) {
			if (weight[i] == 0.0f) {
				boneId[i] = id;
				weight[i] = w;
				return;
			}
		}
	}
};

struct Mesh {
	std::string path;
	std::string texturepath;

	std::vector<Vertex> vertices;
	BufferHandler vertexbuffer;

	std::vector<uint16_t> aiindices;
	std::vector<uint16_t> indices = {
     0, 1, 3,  3, 1, 2,
     4, 5, 6, 6, 7, 4
	};
	BufferHandler indexbuffer;

	glm::mat4 bindtransform;
};

struct BoneInfo {
	glm::mat4 offset;
	glm::mat4 fintransform;

	BoneInfo() {};
	BoneInfo(const glm::mat4& offs) {
		offset = offs;
		fintransform = glm::mat4(0.0);
	}
};

struct Model {
	std::string texturepath;
	std::vector<Mesh*> meshes;

	BufferHandler storagebuffer;
	VkDescriptorSet descritor;

	std::vector<VertexBoneData> vert2bones;
	std::vector<int> meshbase;
	std::map<std::string, int> bonamap;

	std::vector<glm::mat4> bonestransforms;
	std::vector<BoneInfo> boneinfo;

	std::vector<glm::mat4> fintransforms;
};

class MeshBuilder {
public:
	void init(PipelineBuilder& pipeline, TextureBuilder& textb, DeletionQueue* deletor) { pipelinehandler = pipeline; texturehandler = textb; deletorhandler = deletor;};
	void loadmeshes();
	bool loadmeshfromobj(const char* filename, int id);
	Mesh loadmeshassimp(aiMesh *mesh);

	void updatemesh(Mesh& mesh);
	void updatemesh(Mesh* mesh, Positions size, Positions newpos);
	void updatemodel(Model* model);
	void clearmeshes() { meshes.clear();};

    void setvertexbonedefaultdata(Vertex& vertex);

	Mesh* getmesh(int id);

private:
	PipelineBuilder pipelinehandler;
	TextureBuilder  texturehandler;
	DeletionQueue*	deletorhandler;
	
	//Mesh 			triangle;
	std::unordered_map
	<int, Mesh> 	meshes;

};