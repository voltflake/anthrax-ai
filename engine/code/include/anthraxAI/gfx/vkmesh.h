#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/bufferhelper.h"

namespace Gfx
{
    #define BONE_INFLUENCE 4
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;
        // int boneID[4];
        // float weights[4];
    };

    struct MeshPushConstants {
        glm::mat4 model;
        glm::mat4 rendermatrix;

        int objectID;
        int boneind = -1;
        int debug = 0;
        int debugbones = 0;
    };

    struct MeshInfo {
        std::string Path;

        std::vector<Vertex> Vertices;
        BufferHelper::Buffer VertexBuffer;
        
        std::vector<uint16_t> AIindices;
        std::vector<uint16_t> Indices = {
         0, 1, 3,  3, 1, 2,
         4, 5, 6, 6, 7, 4
        };

        BufferHelper::Buffer IndexBuffer;
    };

    class Mesh : public Utils::Singleton<Mesh>
    {
        public:
            void CreateMeshes();
            void UpdateMesh(MeshInfo& mesh);

            MeshInfo* GetMesh() { return &TestMesh; }            
        private:
            MeshInfo TestMesh;
    };
}