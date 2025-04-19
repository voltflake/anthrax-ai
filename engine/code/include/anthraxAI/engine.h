#pragma once

#include "anthraxAI/utils/debug.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/core/scene.h"
#include "anthraxAI/core/audio.h"
#include "anthraxAI/core/imguihelper.h"

#include "anthraxAI/gfx/vkbase.h"

#include <sys/time.h>

enum EngineState {
    ENGINE_STATE_INIT 	= 1 << 0,	/* 0000 0001 */
	ENGINE_STATE_EDITOR = 1 << 1, 	/* 0000 0010 */
	ENGINE_STATE_PLAY 	= 1 << 2,	/* 0000 0100 */
	ENGINE_STATE_RESOURCE_RELOAD  = 1 << 3,	/* 0000 1000 */
	ENGINE_STATE_INTRO = 1 << 4,	/* 0001 0000 */
	ENGINE_STATE_SHADER_RELOAD = 1 << 5,  /* 0010 0000 */
	ENGINE_STATE_EXIT 	= 1 << 6,	/* 0100 0000 */
};

class Engine : public Utils::Singleton<Engine>
{
    public:
        void Init();
        void Run();
        void CleanUp();

        int GetState() const { return State; }
        void SetState(int state) { State |= state; }
        void ClearState(int state) {  Utils::ClearBit(&State, state); }
        void ToggleEditorMode() { Utils::ToggleBit(&State, ENGINE_STATE_EDITOR);  Utils::ToggleBit(&State, ENGINE_STATE_PLAY);}
        void SetEditorMode() { Utils::ClearBit(&State, ENGINE_STATE_INTRO); State |= ENGINE_STATE_EDITOR; }
        void CheckState();

        long long GetTime() const;
        long long GetTimeSinceStart() const { return GetTime() - StartTime; }

        bool OnResize();
    private:
        int State;
        long long StartTime;
};
