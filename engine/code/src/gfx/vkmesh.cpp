#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkrenderer.h"

void Gfx::Mesh::UpdateMesh(MeshInfo& mesh)
{
    VkBufferUsageFlags flags[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
	BufferHelper::CreateBuffer(mesh.VertexBuffer, flags, sizeof(mesh.Vertices[0]) * mesh.Vertices.size(), mesh.Vertices.data());

	VkBufferUsageFlags flags2[2] = {VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT};
	if (mesh.AIindices.empty()) {
		BufferHelper::CreateBuffer(mesh.IndexBuffer, flags2, sizeof(mesh.Indices[0]) * mesh.Indices.size(), mesh.Indices.data());
	}
	else {
		BufferHelper::CreateBuffer(mesh.IndexBuffer, flags2, sizeof(mesh.AIindices[0]) * mesh.AIindices.size(), mesh.AIindices.data());
	}

	Core::Deletor::GetInstance()->Push([=, this]() {
       vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), mesh.VertexBuffer.Buffer, nullptr);
       vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), mesh.VertexBuffer.DeviceMemory, nullptr);
       vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), mesh.IndexBuffer.Buffer, nullptr);
       vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), mesh.IndexBuffer.DeviceMemory, nullptr);
    });
}

void Gfx::Mesh::CreateMeshes()
{
    TestMesh.Path = "placeholder.jpg";//list.second.texturepath;
    TestMesh.Vertices.resize(4);

    TestMesh.Vertices[0].position = {0, 0, 0.0f};
    TestMesh.Vertices[1].position = {0, 0 + Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSize().y, 0.0f};
    TestMesh.Vertices[2].position = {0 + Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSize().x, 0+Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSize().y, 0.0f};
    TestMesh.Vertices[3].position ={0 + Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSize().x, 0, 0.0f};

    TestMesh.Vertices[0].color = { 0.f, 1.f, 0.0f };
    TestMesh.Vertices[1].color = { 0.f, 1.f, 0.0f };
    TestMesh.Vertices[2].color = { 0.f, 1.f, 0.0f };
    TestMesh.Vertices[3].color = { 1.f, 0.f, 0.0f };

    TestMesh.Vertices[0].uv = {0.0f, 0.0f};
    TestMesh.Vertices[1].uv = {0.0f, 1.0f};
    TestMesh.Vertices[2].uv = {1.0f, 1.0f};
    TestMesh.Vertices[3].uv = {1.0f, 0.0f};

    UpdateMesh(TestMesh);
}