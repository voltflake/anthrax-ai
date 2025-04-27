#pragma once

//#define TRACY

#include "anthraxAI/gfx/vkdefines.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyVulkan.hpp"
#include "common/TracyColor.hpp"


/*#define SET_TRACY(x) Utils::Debug::GetInstance()->Tracy = x*/
/*#define ON_TRACY() if (Utils::Debug::GetInstance()->Tracy )*/

/*#define TRACY_NOT_INCLUDED(x) \*/
/*{                           \*/
/*    if (!Utils::Debug::GetInstance()->Tracy ) {    \*/
/*        x;                  \*/
/*    }                       \*/
/*}*/
/**/
/*#define SCOPE_ZONE(s)        \*/
/*{                           \*/
/*        ZoneScopedN(s);     \*/
/*}                             */
/**/
#ifdef TRACY
#define END_FRAME(s)        \
{                           \
    {                       \
        FrameMarkEnd(s);    \
    }                       \
}

#define START_FRAME(s)        \
{                           \
    {                       \
        FrameMarkStart(s);    \
    }                       \
}
#endif
/**/
/*#define VK_ZONE(name, color) \*/
/*{                             \*/
/*    TracyVkZoneC(Gfx::Renderer::GetInstance()->GetTracyContext(), Gfx::Renderer::GetInstance()->GetCmd(), name, color) \*/
/*}                             */
/**/
/*#define VK_COLLECT()         \*/
/*{                             \*/
/*    TracyVkCollect(Gfx::Renderer::GetInstance()->GetTracyContext(), Gfx::Renderer::GetInstance()->GetCmd());   \*/
/*}*/

