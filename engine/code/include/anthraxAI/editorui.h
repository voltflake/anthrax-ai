#pragma once
#include "anthraxAI/vkdefines.h"

class EditorUI
{
    public:
        void beginwindow(const Positions& offset, const Positions& size, ImGuiWindowFlags flags,
                        const std::string& name, bool* act = nullptr);

        void endwindow();
};