#pragma once
#include "anthraxAI/utils/defines.h"
#include <fstream>

namespace Utils
{
    struct Debug : public Utils::Singleton<Debug>
    {
        float FPS;
        float DeltaMs;
        long long AnimStartMs;
        bool Grid = true;
        bool Bones;
        int BoneID = 0;
    };

    static  bool IsBitSet(int state, int bit) { return (state & bit) != 0; }
    static  void ClearBit(int* state, int bit) { (*state &= ~bit); }
    static  void ToogleBit(int* state, int bit) { (*state ^= bit); }

    static inline void ReadFile(const std::string& filename, std::vector<char>& buffer)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            ASSERT(true, "Can't open a file: " + filename);
        }

        size_t filesize = (size_t) file.tellg();
        buffer.resize(filesize);
        file.seekg(0);
        file.read(buffer.data(), filesize);
        file.close();
    }
}
