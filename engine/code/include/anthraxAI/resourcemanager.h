#pragma once

#include "anthraxAI/vkdefines.h"

class ResourceManager
{
public:
    ResourceManager() {};

    void add(int type, Data data) {
        resources[type] = data;
    }

    Data& get(int type) {
        return resources[type];
    }

    ResourcesMap& get() {
        return resources;
    }

    void clear() { resources.clear(); };
private:
	ResourcesMap resources;
};