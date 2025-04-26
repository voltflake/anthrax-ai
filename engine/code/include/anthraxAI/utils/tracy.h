#pragma oncce

#include "anthraxAI/gfx/vkdefines.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyVulkan.hpp"

#define SET_TRACY(x) Utils::Tracy = x
#define ON_TRACY() if (Utils::Tracy)

#define TRACY_NOT_INCLUDED(x) \
{                           \
    if (!Utils::Tracy) {    \
        x;                  \
    }                       \
}

#define SCOPE_ZONE(s)        \
{                           \
    ON_TRACY()              \
    {                       \
        ZoneScopedN(s);     \
    }                       \
}                             

#define END_FRAME(s)        \
{                           \
    ON_TRACY()              \
    {                       \
        FrameMarkEnd(s);    \
    }                       \
}

#define VK_ZONE(i, name) \
{                             \
    ON_TRACY()                \
    {                         \
        TracyVkZone(Gfx::Vulkan::GetInstance()->TracyVk[i], Gfx::Renderer::GetInstance()->GetCmd(), name) \
    }                         \
}                             \

namespace Utils
{
    static inline bool Tracy = false;
}
