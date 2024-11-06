#pragma once
#include "anthraxAI/utils/defines.h"
namespace Utils
{
    struct Debug : public Utils::Singleton<Debug>
 {
        float FPS;
        float DeltaMs;
        bool Grid = false;
    };


    static  bool IsBitSet(int state, int bit) { return (state & bit) != 0; } 
    static  void ClearBit(int* state, int bit) { (*state &= ~bit); } 
    static  void ToogleBit(int* state, int bit) { (*state ^= bit); } 
}
