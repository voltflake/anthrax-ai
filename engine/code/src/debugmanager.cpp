#include "anthraxAI/debugmanager.h"


void Debug::DebugManager::calculateFPS(std::chrono::duration<double, std::milli>& delta) {
    if (delta.count() < (1000.0f / MAX_FPS + 2.0f / MAX_FPS)) {
        std::chrono::duration<double, std::milli> deltams((1000.0f / MAX_FPS) + (2.0f / MAX_FPS) - delta.count());
        auto msduration = std::chrono::duration_cast<std::chrono::milliseconds>(deltams);
        std::this_thread::sleep_for(std::chrono::milliseconds(msduration.count()));
        fps = 1000.0f / deltams.count();
    }
}

void Debug::DebugManager::calculateFPS(long long delta)
{
}
