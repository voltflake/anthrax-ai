#include "anthraxAI/engine.h"

void Engine::Init()
{
    State = ENGINE_STATE_INIT;
	ASSERT(State != ENGINE_STATE_INIT, "How is it possible?");

#ifdef AAI_LINUX
    Core::WindowManager::GetInstance()->InitLinuxWindow();
#else
    Core::WindowManager::GetInstance()->InitWindowsWindow();
#endif

    Gfx::Vulkan::GetInstance()->Init();

    Core::Scene::GetInstance()->LoadResources();

    State = ENGINE_STATE_EDITOR;
}

void Engine::Run()
{
#ifdef AAI_LINUX
    Core::WindowManager::GetInstance()->RunLinux();
#else
    Core::WindowManager::GetInstance()->RunWindows();
#endif

    if (State & ENGINE_STATE_EXIT) {
        CleanUp();
    }
}

void Engine::CleanUp()
{
    Gfx::Vulkan::GetInstance()->CleanUp();
}