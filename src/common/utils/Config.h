#pragma once

#include "../common.h"
#include <string>
#include <map>

class ConfigNode {

public:
    enum class Type {
        STRING, CONTAINER
    };

    std::string name;
    Type type;

    ConfigNode(std::string name, Type type) : name(name), type(type) {};

    operator std::string() const;
    ConfigNode* operator[](const std::string& name);
    std::string getString(const std::string& name, std::string d) {
        ConfigNode* n = (*this)[name];
        if (n == null) return d;
        return *n;
    };

};

class StringConfigNode : public ConfigNode {

public:
    std::string val;

    StringConfigNode(std::string name, std::string val) : ConfigNode(name, ConfigNode::Type::STRING), val(val) {};

};

class ContainerConfigNode : public ConfigNode {

public:
    std::map<std::string, ConfigNode*> val;

    ContainerConfigNode(std::string name) : ConfigNode(name, ConfigNode::Type::CONTAINER) {};

};

class Config {

protected:
    void print(ConfigNode* node, int scope);

public:
    ContainerConfigNode* mainNode;

    Config(std::string file);

    void print() {
        for (auto& e : mainNode->val) {
            print(e.second, 0);
        }
    };

    ConfigNode* operator[](const std::string& name) { return (*mainNode)[name]; };
    std::string getString(const std::string& name, std::string d) { return mainNode->getString(name, d); };

};