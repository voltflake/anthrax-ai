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
	float w, h;
	w = texturehandler.gettexture("first")->w;
	h = texturehandler.gettexture("first")->h;

	//float k = w / h;

	// w = 2.0 * (w / WindowExtend.width) - 1.0;
	// h = 2.0 * (h / WindowExtend.height) - 1.0;
	std::cout << w << "||" << h <<"\n";
	Mesh 			triangle;

	triangle.vertices.resize(4);
	

	triangle.vertices[0].position = {0, 0, 0.0f};
	triangle.vertices[1].position = {0, 0 + h, 0.0f};
	triangle.vertices[2].position = {0 + w, 0 + h, 0.0f};
	triangle.vertices[3].position ={0 + w, 0, 0.0f};
	// triangle.vertices[0].position = {-1.0f, -1.0f, 0.0f};
	// triangle.vertices[1].position = {0.0f, -1.0f, 0.0f};
	// triangle.vertices[2].position = {0.0f, 0.0f, 0.0f};
	// triangle.vertices[3].position ={-1.0f, 0.0f, 0.0f};

	triangle.vertices[0].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[1].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[2].color = { 0.f, 1.f, 0.0f };
	triangle.vertices[3].color = { 1.f, 0.f, 0.0f };

	triangle.vertices[0].uv = {0.0f, 0.0f};
	triangle.vertices[1].uv = {0.0f, 1.0f};
	triangle.vertices[2].uv = {1.0f, 1.0f};
	triangle.vertices[3].uv = {1.0f, 0.0f};
		triangle.indices.resize(6);

	triangle.indices = {
     0, 1, 3,  3, 1, 2,
     4, 5, 6, 6, 7, 4
	};
	updatemesh(triangle);

	meshes["triangle"] = triangle;

	w = texturehandler.gettexture("sec")->w;
	h = texturehandler.gettexture("sec")->h;

	triangle.vertices[0].position = {600, 600, 0.0f};
	triangle.vertices[1].position = {600, 600 + h, 0.0f};
	triangle.vertices[2].position = {600 + w, 600 + h, 0.0f};
	triangle.vertices[3].position ={600 + w, 600, 0.0f};

	// triangle.indices = {

    //  4, 5, 6, 6, 7, 4
	// };

	updatemesh(triangle);

	meshes["triangle_sec"] = triangle;
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