#pragma once
#include "anthraxAI/utils/defines.h"
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

    

}
