#include "anthraxAI/vkmesh.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader.h>

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


void MeshBuilder::setvertexbonedefaultdata(Vertex& vertex)
{
    for (int i = 0; i < BONE_INFLUENCE; i++)
    {
        vertex.boneID[i] = -1;
        vertex.weights[i] = 0.0f;
    }
}

Mesh MeshBuilder::loadmeshassimp(aiMesh *mesh)
{
	Mesh model;

	for(int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        //setvertexbonedefaultdata(vertex);
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

bool MeshBuilder::loadmeshfromobj(const char* filename, int id)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}
 
	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}
	Mesh model;
	 // Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            //hardcode loading to triangles
			int fv = 3;

			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
            	tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];


                //copy it into our vertex
				Vertex new_vert;
				new_vert.position.x = vx;
				new_vert.position.y = vy;
				new_vert.position.z = vz;

				new_vert.normal.x = nx;
				new_vert.normal.y = ny;
                new_vert.normal.z = nz;

				new_vert.uv = {
					attrib.texcoords[2 * idx.texcoord_index + 0],
    				1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]
				};
				new_vert.color = {1.0f, 1.0f, 1.0f};
                new_vert.color = new_vert.normal;

				model.vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}
	
	updatemesh(model);
	meshes[id] = model;
	std::cout << "mesh name: monkey loaded succesfully" << "\n";

    return true;
}

void MeshBuilder::loadmeshes(){
	float w, h;

	for (auto list : texturehandler.resources) {
		if (list.first >= TYPE_MODEL || list.second.texturepath == "") {
            continue;
        }
		h = texturehandler.gettexture(list.second.texturepath)->h;
		w = texturehandler.gettexture(list.second.texturepath)->w;

		Mesh 			triangle;

		triangle.path = list.second.texturepath;
		triangle.vertices.resize(4);

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

		std::cout << "mesh name: " << list.second.texturepath << "\n";
		std::cout << "mesh id: " << list.first << "\n";

		meshes[list.first] = triangle;
	}
}

void MeshBuilder::updatemodel(Model* model)
{
	for (int i = 0; i < model->meshes.size(); i++) {
		updatemesh(*model->meshes[i]);
	}
}

void MeshBuilder::updatemesh(Mesh* mesh, Positions size, Positions pos) {
	float w, h;
	
	float x = pos.x;
	float y = pos.y;

	mesh->vertices[0].position = {x , y, 0.0f};
	mesh->vertices[1].position = {x, y + size.y, 0.0f};
	mesh->vertices[2].position = {x + size.x, y + size.y, 0.0f};
	mesh->vertices[3].position = {x + size.x, y, 0.0f};

// broken for 2d animation - vkAllocateMemory: Number of currently valid memory objects is not less than maxMemoryAllocationCount (4096)
	updatemesh(*mesh);
}

void MeshBuilder::updatemesh(Mesh& mesh){

	BufferBuilder buffer;

	VkBufferUsageFlags flags[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};

	buffer.createbuffer(pipelinehandler.getrenderer(), mesh.vertexbuffer, flags, sizeof(mesh.vertices[0]) * mesh.vertices.size(), mesh.vertices.data());

	VkBufferUsageFlags flags2[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT};
	if (mesh.aiindices.empty()) {
		buffer.createbuffer(pipelinehandler.getrenderer(), mesh.indexbuffer, flags2, sizeof(mesh.indices[0]) * mesh.indices.size(), mesh.indices.data());
	}
	else {
		buffer.createbuffer(pipelinehandler.getrenderer(), mesh.indexbuffer, flags2, sizeof(mesh.aiindices[0]) * mesh.aiindices.size(), mesh.aiindices.data());
	}

	deletorhandler->pushfunction([=]() {
       vkDestroyBuffer(pipelinehandler.getdevice()->getlogicaldevice(), mesh.vertexbuffer.buffer, nullptr);
       vkFreeMemory(pipelinehandler.getdevice()->getlogicaldevice(), mesh.vertexbuffer.devicememory, nullptr);
       vkDestroyBuffer(pipelinehandler.getdevice()->getlogicaldevice(), mesh.indexbuffer.buffer, nullptr);
       vkFreeMemory(pipelinehandler.getdevice()->getlogicaldevice(), mesh.indexbuffer.devicememory, nullptr);
    });
}