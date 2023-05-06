#include "../includes/vkmesh.h"

Mesh* MeshBuilder::getmesh(const std::string& name)
{
	auto it = meshes.find(name);
	if (it == meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


void MeshBuilder::loadmeshes(){


	triangle.vertices.resize(4);
	
	triangle.vertices[0].position = {-0.5f, -0.5f, 0.0f};
	triangle.vertices[1].position = {0.5f, -0.5f, 0.0f};
	triangle.vertices[2].position = {0.5f, 0.5f, 0.0f};
	triangle.vertices[3].position ={-0.5f, 0.5f, 0.0f};

	triangle.vertices[0].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[1].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[2].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[3].color = { 1.f, 0.f, 0.0f };

	triangle.vertices[0].uv = {1.0f, 0.0f};
triangle.vertices[1].uv = {0.0f, 0.0f};
triangle.vertices[2].uv = {0.0f, 1.0f};
triangle.vertices[3].uv = {1.0f, 1.0f};


	updatemesh(triangle);

	meshes["triangle"] = triangle;
}

void MeshBuilder::updatemesh(Mesh& mesh){
	
	BufferBuilder buffer;
	
	VkBufferUsageFlags flags[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};

	buffer.createbuffer(pipelinehandler.getrenderer(), mesh.vertexbuffer, flags, sizeof(mesh.vertices[0]) * mesh.vertices.size(), mesh.vertices.data());

	VkBufferUsageFlags flags2[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT};
	buffer.createbuffer(pipelinehandler.getrenderer(), mesh.indexbuffer, flags2, sizeof(mesh.indices[0]) * mesh.indices.size(), mesh.indices.data());
	

	deletorhandler.pushfunction([=]() {
        vkDestroyBuffer(pipelinehandler.getdevice().getlogicaldevice(), mesh.vertexbuffer.buffer, nullptr);
        vkFreeMemory(pipelinehandler.getdevice().getlogicaldevice(), mesh.vertexbuffer.devicememory, nullptr);
       vkDestroyBuffer(pipelinehandler.getdevice().getlogicaldevice(), mesh.indexbuffer.buffer, nullptr);
       vkFreeMemory(pipelinehandler.getdevice().getlogicaldevice(), mesh.indexbuffer.devicememory, nullptr);
 
    });
}