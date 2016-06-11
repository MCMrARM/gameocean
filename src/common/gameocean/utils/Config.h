#pragma once

#include "../common.h"
#include <string>
#include <map>
#include <memory>
#include <iostream>

class ContainerConfigNode;
class ConfigNode {

public:
    enum class Type {
        STRING, CONTAINER
    };

    std::string name;
    Type type;

    ConfigNode(std::string name, Type type) : name(name), type(type) { }

    operator std::string() const;
    std::shared_ptr<ConfigNode> operator[](const std::string &name);
    std::shared_ptr<ContainerConfigNode> getContainer(const std::string &name) {
        std::shared_ptr<ContainerConfigNode> n = std::static_pointer_cast<ContainerConfigNode>((*this)[name]);
        return n;
    }
    std::string getString(const std::string &name, std::string d) {
        std::shared_ptr<ConfigNode> n = (*this)[name];
        if (n == nullptr) return d;
        return *n;
    }
    int getInt(const std::string &name, int d);
    float getFloat(const std::string &name, float d);
    bool getBool(const std::string &name, bool d) {
        std::shared_ptr<ConfigNode> n = (*this)[name];
        if (n == nullptr) return d;
        std::string nt = *n;
        if (nt == "y" || nt == "yes" || nt == "true" || nt == "on" || nt == "enabled") {
            return true;
        } else if (nt == "n" || nt == "no" || nt == "false" || nt == "off" || nt == "disabled") {
            return false;
        }
        return d;
    }

};

class StringConfigNode : public ConfigNode {

public:
    std::string val;

    StringConfigNode(std::string name, std::string val) : ConfigNode(name, ConfigNode::Type::STRING), val(val) { }

};

class ContainerConfigNode : public ConfigNode {

public:
    std::map<std::string, std::shared_ptr<ConfigNode>> val;

    ContainerConfigNode(std::string name) : ConfigNode(name, ConfigNode::Type::CONTAINER) { }

};

class Config {

protected:
    void print(std::shared_ptr<ConfigNode> node, int scope);

public:
    std::shared_ptr<ContainerConfigNode> mainNode;

    Config(std::istream &is);

    void print() {
        for (auto &e : mainNode->val) {
            print(e.second, 0);
        }
    }

    inline std::shared_ptr<ConfigNode> operator[](const std::string &name) { return (*mainNode)[name]; }
    inline std::shared_ptr<ContainerConfigNode> getContainer(const std::string &name) { return mainNode->getContainer(name); }
    inline std::string getString(const std::string &name, std::string d) { return mainNode->getString(name, d); }
    inline int getInt(const std::string &name, int d) { return mainNode->getInt(name, d); }
    inline float getFloat(const std::string &name, bool d) { return mainNode->getFloat(name, d); }
    inline bool getBool(const std::string &name, bool d) { return mainNode->getBool(name, d); }

};