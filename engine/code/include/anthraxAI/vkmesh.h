#pragma once

#include "anthraxAI/animator.h"
#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkpipeline.h"
#include "anthraxAI/vkbuffer.h"
#include "anthraxAI/vktexture.h"

#include <vector>

struct Mesh {
	std::string path;
	std::vector<Vertex> vertices;
	BufferHandler vertexbuffer;

	std::vector<uint16_t> indices = {
     0, 1, 3,  3, 1, 2,
     4, 5, 6, 6, 7, 4
	};
	BufferHandler indexbuffer;
};

class MeshBuilder {
public:
	void init(PipelineBuilder& pipeline, TextureBuilder& textb, DeletionQueue* deletor) { pipelinehandler = pipeline; texturehandler = textb; deletorhandler = deletor;};
	void loadmeshes();
	bool loadmeshfromobj(const char* filename, int id);

	void updatemesh(Mesh& mesh);
	void updatemesh(Mesh* mesh, Positions size, Positions newpos);

	void clearmeshes() { meshes.clear();};

	Mesh* 		getmesh(int id);

private:
	PipelineBuilder pipelinehandler;
	TextureBuilder  texturehandler;
	DeletionQueue*	deletorhandler;
	
	//Mesh 			triangle;
	std::unordered_map
	<int, Mesh> 	meshes;

};