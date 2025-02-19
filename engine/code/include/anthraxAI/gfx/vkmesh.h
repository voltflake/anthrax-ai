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
        glm::vec4 instanceind;
        glm::vec4 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;
        float weights[4];
        int boneID[4];
    };

    struct MeshPushConstants {
        int texturebind = 0;
        int storagebind = 0;
        int instancebind = 0;
        int bufferbind = 0; 
        int objectID = 0; 
        int selected = 0;
        int boneID = 0;
        int gizmo = 0;
    };

    struct MeshInfo {
        std::string Path = "";

        std::vector<Vertex> Vertices;
        BufferHelper::Buffer VertexBuffer;
        
        std::vector<uint16_t> AIindices;
        std::vector<uint16_t> Indices = {
            0, 1, 3,  3, 1, 2,
            4, 5, 6, 6, 7, 4
        };

        BufferHelper::Buffer IndexBuffer;

        void Clean();
    };
    typedef std::unordered_map<std::string, MeshInfo> MeshMap;

    class Mesh : public Utils::Singleton<Mesh>
    {
        public:
            void CleanAll();

            void CreateMeshes();
            void CreateMesh(aiMesh* aimesh, Gfx::MeshInfo* meshinfo);

            void Update(MeshInfo& mesh);
            void UpdateMesh(Gfx::MeshInfo* meshinfo);
            MeshInfo* GetMesh(const std::string& name);   
        private:
            void SetVertexBoneDefaultData(Gfx::Vertex& vertex);
            MeshMap Meshes;
    };
}
