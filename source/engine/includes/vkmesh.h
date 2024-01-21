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

	std::vector<uint16_t> indices = {
     0, 1, 3,  3, 1, 2,
     4, 5, 6, 6, 7, 4
	};
	BufferHandler indexbuffer;
};

class MeshBuilder {
public:
	void init(PipelineBuilder& pipeline,TextureBuilder& textb, DeletionQueue* deletor) { pipelinehandler = pipeline; texturehandler = textb; deletorhandler = deletor;};
	void loadmeshes(std::unordered_map<std::string, Positions>& resources);
	void updatemesh(Mesh& mesh);
	void updateplayermesh(Mesh* mesh, std::string texture, int newx, int newy);

	void clearmeshes() { meshes.clear();};

	Mesh* 		getmesh(const std::string& name);

private:
	PipelineBuilder pipelinehandler;
	TextureBuilder  texturehandler;
	DeletionQueue*	deletorhandler;
	
	//Mesh 			triangle;
	std::unordered_map
	<std::string, Mesh> 	meshes;

};