
#pragma once

#include "anthraxAI/vkdefines.h"

namespace Debug
{
    struct DebugAnim 
    {
	    int scale = 1;
	    VkDescriptorSet desc;
	    std::string path;
    };

    struct Mouse
    {
        Positions 	pos = {0, 0};
        Positions 	posdelta = {0, 0};
        Positions 	begin = {0, 0};
        MouseState	state = MOUSE_IDLE;
    };
    
    class DebugManager
    {
        public:
            DebugManager() {};
            ~DebugManager() {};

            void clear()                { ImGuiAnim.clear(); }

            int                         boneID = 0;
            bool                        animprepared = false;
            bool                        bones = false;
            float                       animspeed = 1.0;
            bool						freemove = false;

            ImGuiStyle 					EditorStyle;
            ImGuiStyle 					TextDisplayStyle;
            std::vector<DebugAnim> 		ImGuiAnim;

            std::chrono::duration<double, std::milli> deltatime;
            long long                   startms;
            float 						fps = 0;
            void 						calculateFPS(std::chrono::duration<double, std::milli>& delta);
            void                        calculateFPS(long long delta);
    
    };
}
