#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/core/scene.h"

#include "anthraxAI/gfx/vkbase.h"

enum EngineState {
    ENGINE_STATE_INIT 	= 1 << 0,	/* 0000 0001 */
	ENGINE_STATE_EDITOR = 1 << 1, 	/* 0000 0010 */
	ENGINE_STATE_EXIT 	= 1 << 2,	/* 0000 0100 */
};

class Engine : public Utils::Singleton<Engine>
{
    public:
        void Init();
        void Run();
        void CleanUp();

        EngineState GetState() const { return State; }
        void SetState(EngineState state) { State = state; }

    private:
        EngineState State;
};