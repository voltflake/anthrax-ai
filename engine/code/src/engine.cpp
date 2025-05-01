#include "anthraxAI/engine.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/utils/tracy.h"
#include <chrono>
#include <cstdio>

long long Engine::GetTime() const {
#ifdef AAI_WINDOWS
    return GetTickCount();
#else
    std::chrono::duration<long int, std::ratio<1, 1000000000>> t = Timer.now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
#endif
}

void Engine::CheckState()
{
    if (Utils::IsBitSet(State, ENGINE_STATE_INTRO)) {
        SetEditorMode();
    }
    else {
        ToggleEditorMode();
    }

}

bool Engine::OnResize()
{
    return Gfx::Vulkan::GetInstance()->OnResize();
}

void Engine::Init()
{
    SetState(ENGINE_STATE_INIT);
  	ASSERT(!Utils::IsBitSet(State, ENGINE_STATE_INIT), "How is it possible?");

#ifdef AAI_LINUX
    Core::WindowManager::GetInstance()->InitLinuxWindow();
#else
    Core::WindowManager::GetInstance()->InitWindowsWindow();
#endif

    Core::Scene::GetInstance()->Init();
#ifndef TRACY
    Core::Audio::GetInstance()->Init();
#endif
    Gfx::Vulkan::GetInstance()->Init();

    Core::Scene::GetInstance()->InitModules();

    Core::ImGuiHelper::GetInstance()->Init();

    SetState(ENGINE_STATE_INTRO);
}

void Engine::Run()
{
    StartTime = GetTime();
#ifdef AAI_LINUX
    Core::WindowManager::GetInstance()->RunLinux();
#else
    Core::WindowManager::GetInstance()->RunWindows();
#endif

    if (Utils::IsBitSet(State, ENGINE_STATE_EXIT)) {
        CleanUp();
    }
}

void Engine::CleanUp()
{
    Gfx::Vulkan::GetInstance()->CleanUp();
}
