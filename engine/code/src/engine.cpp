#include "anthraxAI/engine.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/utils/tracy.h"
#include <cstdio>

long long Engine::GetTime() const {
#ifdef AAI_WINDOWS
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);
    long long tim = t.tv_sec * 1000 + t.tv_usec / 1000;
	  return (tim);
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

void Engine::Init(char** argv)
{
    if (argv[1] && strcmp(argv[1], "tracy") == 0) {
        SET_TRACY(true);
    }
    SetState(ENGINE_STATE_INIT);
  	ASSERT(!Utils::IsBitSet(State, ENGINE_STATE_INIT), "How is it possible?");

#ifdef AAI_LINUX
    Core::WindowManager::GetInstance()->InitLinuxWindow();
#else
    Core::WindowManager::GetInstance()->InitWindowsWindow();
#endif

    Core::Scene::GetInstance()->Init();
    TRACY_NOT_INCLUDED(Core::Audio::GetInstance()->Init());

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
