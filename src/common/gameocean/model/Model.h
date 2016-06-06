#pragma once

#include <map>
#include <string>
#include <vector>
#include "../math/AABB.h"

class Model {

public:
    static std::map<std::string, Model *> models;

    static Model *getModel(std::string name);

    std::vector<AABB> aabbs;

};


