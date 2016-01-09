#include "Config.h"

#include <iostream>
#include <fstream>
#include "StringUtils.h"

Config::Config(std::istream& is) {
    if (!is)
        return;

    mainNode = std::make_shared<ContainerConfigNode> ("");

    std::string line;

    std::vector<std::shared_ptr<ContainerConfigNode>> scope;
    scope.push_back(mainNode);
    while (getline(is, line)) {
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

        std::shared_ptr<ContainerConfigNode> cur = scope.back();
        if (val.length() <= 0) {
            std::shared_ptr<ContainerConfigNode> el (new ContainerConfigNode(key));
            cur->val[key] = el;
            scope.push_back(el);
        } else {
            cur->val[key] = std::make_shared<StringConfigNode> (key, val);
        }
    }
}

void Config::print(std::shared_ptr<ConfigNode> node, int scope) {
    for (int i = 0; i < scope; i++) {
        std::cout << "  ";
    }
    std::cout << node->name << ": ";
    if (node->type == ConfigNode::Type::STRING) {
        std::cout << std::static_pointer_cast<StringConfigNode>(node)->val;
    }
    std::cout << "\n";
    if (node->type == ConfigNode::Type::CONTAINER) {
        for (auto& e : (std::static_pointer_cast<ContainerConfigNode>(node))->val) {
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

std::shared_ptr<ConfigNode> ConfigNode::operator[](const std::string& name) {
    if (type == Type::CONTAINER) {
        ContainerConfigNode* t = (ContainerConfigNode*) this;
        if (t->val.count(name) <= 0) {
            return nullptr;
        }
        return t->val.at(name);
    }
    return nullptr;
}

int ConfigNode::getInt(const std::string &name, int d) {
    std::shared_ptr<ConfigNode> n = (*this)[name];
    if (n == nullptr) return d;
    std::string nt = *n;
    return StringUtils::asInt(nt, d);
}