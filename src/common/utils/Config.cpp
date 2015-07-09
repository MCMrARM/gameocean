#include "Config.h"

#include <iostream>
#include <fstream>
#include "StringUtils.h"

Config::Config(std::string file) {
    std::ifstream fin (file);

    mainNode = new ContainerConfigNode("");

    if (!fin) return;

    std::string line;

    std::vector<ContainerConfigNode*> scope;
    scope.push_back(mainNode);
    while (getline(fin, line)) {
        if (line.length() <= 0) continue;
        std::size_t iof = line.find("#");
        if (iof != std::string::npos) {
            line = line.substr(0, iof);
        }
        if (line.length() <= 0) continue;

        // get space count on the line
        int spaceNum = line.find_first_not_of(" ");
        int lScope = spaceNum / 2;
        for (int i = scope.size() - 1; i > lScope; i--) {
            scope.pop_back();
        }

        line = line.substr(spaceNum);
        iof = line.find(":");
        if (iof == std::string::npos) continue;
        std::string key = StringUtils::trim(line.substr(0, iof));
        std::string val = StringUtils::trim(line.substr(iof + 1));

        ContainerConfigNode *cur = scope.back();
        if (val.length() <= 0) {
            ContainerConfigNode *el = new ContainerConfigNode(key);
            cur->val[key] = el;
            scope.push_back(el);
        } else {
            cur->val[key] = new StringConfigNode(key, val);
        }
    }
}

void Config::print(ConfigNode *node, int scope) {
    for (int i = 0; i < scope; i++) {
        std::cout << "  ";
    }
    std::cout << node->name << ": ";
    if (node->type == ConfigNode::Type::STRING) {
        std::cout << ((StringConfigNode*) node)->val;
    }
    std::cout << "\n";
    if (node->type == ConfigNode::Type::CONTAINER) {
        ContainerConfigNode* c = (ContainerConfigNode*) node;
        for (auto& e : c->val) {
            Config::print(e.second, scope + 1);
        }
    }
}

ConfigNode::operator std::string() const {
    if (type == Type::STRING) {
        return ((StringConfigNode*) this)->val;
    }
    return "";
}

ConfigNode* ConfigNode::operator[](const std::string& name) {
    if (type == Type::CONTAINER) {
        ContainerConfigNode* t = (ContainerConfigNode*) this;
        if (t->val.count(name) <= 0) {
            return null;
        }
        return t->val.at(name);
    }
    return null;
}