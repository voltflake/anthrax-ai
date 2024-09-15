#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/bufferhelper.h"

namespace Gfx
{
    struct FrameData {
        VkSemaphore PresentSemaphore, RenderSemaphore;
        VkFence RenderFence;	
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
    };
    typedef std::array<FrameData, MAX_FRAMES> FrameArray;

    class Renderer : public Utils::Singleton<Renderer> 
    {
        public:
            void CreateCommands();
            void CreateRenderTargets();

            void AllocateRTMemory(RenderTarget* rt);
        private:
            RenderTarget* DepthRT;

        	FrameArray Frames;
    };
}