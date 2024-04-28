#include "../includes/vkengine.h"

void Engine::processanimation() {
    for (std::unordered_map<int, Data>::iterator it = resources.begin(); it != resources.end(); ++it) {
        if (it->second.animation) {
            Builder.updateanimation(Builder.getmesh(it->first), it->first);
        }
    }
}