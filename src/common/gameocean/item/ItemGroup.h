#pragma once

#include "BlockGroup.h"
#include <string>
#include <map>

class ItemGroup {

private:
    static std::map<std::string, ItemGroup *> groups;

public:
    static ItemGroup *get(std::string group) {
        if (groups.count(group) > 0) {
            return groups.at(group);
        }
        groups[group] = new ItemGroup(group);
        return groups[group];
    }

    ItemGroup(std::string name) : name(name) { }
    std::string name;

};