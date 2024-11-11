#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/bufferhelper.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>

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

        int texturebind = 0;
        int storagebind = 0;
        int bufferbind = 0; 
        int objectID = 0; 
        
        int selected = 0;
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
    typedef std::unordered_map<std::string, MeshInfo> MeshMap;

    class Mesh : public Utils::Singleton<Mesh>
    {
        public:
            void CreateMeshes();
            void CreateMesh(aiMesh* aimesh, Gfx::MeshInfo* meshinfo);

            void UpdateMesh(MeshInfo& mesh);

            MeshInfo* GetMesh(const std::string& name);   
        private:
            MeshMap Meshes;
    };
}
