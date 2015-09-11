#pragma once

#include "BlockGroup.h"
#include <string>
#include <map>

class BlockGroup {

private:
    static std::map<std::string, BlockGroup*> groups;

public:
    static BlockGroup* get(std::string group) {
        if (groups.count(group) > 0) {
            return groups.at(group);
        }
        groups[group] = new BlockGroup(group);
        return groups[group];
    };

    BlockGroup(std::string name) : name(name) {};
    std::string name;

};