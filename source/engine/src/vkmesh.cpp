#include "../includes/vkmesh.h"

Mesh* MeshBuilder::getmesh(int id)
{
	auto it = meshes.find(id);
	if (it == meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


void MeshBuilder::loadmeshes(){
	float w, h;

	for (auto list : texturehandler.resources) {
		if (list.second.texturepath == "") {
            continue;
        }
		h = texturehandler.gettexture(list.second.texturepath)->h;
		w = texturehandler.gettexture(list.second.texturepath)->w;

		Mesh 			triangle;

		triangle.path = list.second.texturepath;
		triangle.vertices.resize(4);

		   std::cout << (list.second.pos.x ) << "!www!" << w << "\n";
             std::cout << (list.second.pos.y ) << "!hhh!" << h << "\n";

		triangle.vertices[0].position = {list.second.pos.x, list.second.pos.y, 0.0f};
		triangle.vertices[1].position = {list.second.pos.x, list.second.pos.y + h, 0.0f};
		triangle.vertices[2].position = {list.second.pos.x + w, list.second.pos.y + h, 0.0f};
		triangle.vertices[3].position ={list.second.pos.x + w, list.second.pos.y, 0.0f};

		triangle.vertices[0].color = { 0.f, 1.f, 0.0f };
		triangle.vertices[1].color = { 0.f, 1.f, 0.0f };
		triangle.vertices[2].color = { 0.f, 1.f, 0.0f };
		triangle.vertices[3].color = { 1.f, 0.f, 0.0f };

		triangle.vertices[0].uv = {0.0f, 0.0f};
		triangle.vertices[1].uv = {0.0f, 1.0f};
		triangle.vertices[2].uv = {1.0f, 1.0f};
		triangle.vertices[3].uv = {1.0f, 0.0f};

		updatemesh(triangle);

		meshes[list.first] = triangle;
	}

}

void MeshBuilder::updatemesh(Mesh* mesh, int id, Positions newpos) {
	float w, h;

	w = texturehandler.gettexture(texturehandler.resources[id].texturepath)->w;
	h = texturehandler.gettexture(texturehandler.resources[id].texturepath)->h;

	float x = newpos.x;
	float y = newpos.y;
        // std::cout << x << "|NEW|" << y << "| |\n";

	mesh->vertices[0].position = {x , y, 0.0f};
	mesh->vertices[1].position = {x, y + h, 0.0f};
	mesh->vertices[2].position = {x + w, y + h, 0.0f};
	mesh->vertices[3].position = {x + w, y, 0.0f};

	updatemesh(*mesh);
}

void MeshBuilder::updatemesh(Mesh& mesh){

	BufferBuilder buffer;

	VkBufferUsageFlags flags[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};

	buffer.createbuffer(pipelinehandler.getrenderer(), mesh.vertexbuffer, flags, sizeof(mesh.vertices[0]) * mesh.vertices.size(), mesh.vertices.data());

	VkBufferUsageFlags flags2[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT};
	buffer.createbuffer(pipelinehandler.getrenderer(), mesh.indexbuffer, flags2, sizeof(mesh.indices[0]) * mesh.indices.size(), mesh.indices.data());

	deletorhandler->pushfunction([=]() {
       vkDestroyBuffer(pipelinehandler.getdevice()->getlogicaldevice(), mesh.vertexbuffer.buffer, nullptr);
       vkFreeMemory(pipelinehandler.getdevice()->getlogicaldevice(), mesh.vertexbuffer.devicememory, nullptr);
       vkDestroyBuffer(pipelinehandler.getdevice()->getlogicaldevice(), mesh.indexbuffer.buffer, nullptr);
       vkFreeMemory(pipelinehandler.getdevice()->getlogicaldevice(), mesh.indexbuffer.devicememory, nullptr);
    });
}