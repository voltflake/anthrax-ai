#include "anthraxAI/engine.h"
#include "anthraxAI/utils/debug.h"

long long Engine::GetTime() const {
#ifdef _WIN32
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);
    long long tim = t.tv_sec * 1000 + t.tv_usec / 1000;
	  return tim;
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

#ifdef __linux__
    Core::WindowManager::GetInstance()->InitLinuxWindow();
#else
    Core::WindowManager::GetInstance()->InitWindowsWindow();
#endif

    Core::Scene::GetInstance()->Init();
    Core::Audio::GetInstance()->Init();

    Gfx::Vulkan::GetInstance()->Init();

    Core::Scene::GetInstance()->InitModules();

    Core::ImGuiHelper::GetInstance()->Init();

    SetState(ENGINE_STATE_INTRO);
}

void Engine::Run()
{
    StartTime = GetTime();
#ifdef __linux__
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
