#pragma once

#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkpipeline.h"
#include "vkbuffer.h"
#include "vktexture.h"

#include <vector>

struct Mesh {
	std::vector<Vertex> vertices;
	BufferHandler vertexbuffer;

	std::vector<uint16_t> indices;
	BufferHandler indexbuffer;
};

class MeshBuilder {
public:
	void init(PipelineBuilder& pipeline,TextureBuilder& textb, DeletionQueue& deletor) { pipelinehandler = pipeline; texturehandler = textb; deletorhandler = deletor;};
	void loadmeshes();
	void updatemesh(Mesh& mesh);

	Mesh* 		getmesh(const std::string& name);

private:
	PipelineBuilder pipelinehandler;
	TextureBuilder  texturehandler;
	DeletionQueue	deletorhandler;
	
	//Mesh 			triangle;
	std::unordered_map
	<std::string, Mesh> 	meshes;

};