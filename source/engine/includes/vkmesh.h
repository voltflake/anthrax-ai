#pragma once

#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkpipeline.h"
#include "vkbuffer.h"
#include <vector>

struct Mesh {
	std::vector<Vertex> vertices;
	BufferHandler vertexbuffer;

	std::vector<uint16_t> indices = {
     0, 1, 2, 2, 3, 0,
     4, 5, 6, 6, 7, 4
	};
	BufferHandler indexbuffer;
};

class MeshBuilder {
public:
	void init(PipelineBuilder& pipeline, DeletionQueue& deletor) { pipelinehandler = pipeline; deletorhandler = deletor;};
	void loadmeshes();
	void updatemesh(Mesh& mesh);

	Mesh* 		getmesh(const std::string& name);

private:
	PipelineBuilder pipelinehandler;
	DeletionQueue	deletorhandler;
	
	Mesh 			triangle;
	std::unordered_map
	<std::string, Mesh> 	meshes;

};