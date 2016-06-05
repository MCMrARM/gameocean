#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * This class is responsible for a single permissions. The permission is registered automatically and removed when the
 * object is destroyed.
 */
class Permission {

protected:
    static std::map<std::string, Permission*> permissions;

    static std::set<Permission*> defaultPermissions;
    static std::set<Permission*> operatorPermissions;

public:
    static Permission* getPermission(std::string name);
    static void initializeDefaultPermissions();

    // default permissions
    static Permission* defaultCommands;
    static Permission* defaultPlayerCommands;
    static Permission* defaultOperatorCommands;
    static Permission* helpCommand;
    static Permission* suicideCommand;
    static Permission* stopServerCommand;
    static Permission* reloadServerCommand;
    static Permission* serverVersionCommand;
    static Permission* pingCommand;
    static Permission* teleportPlayerCommand;
    static Permission* giveItemCommand;
    static Permission* worldTimeCommand;

    static inline std::set<Permission*> const& getPlayerPermissions() {
        return defaultPermissions;
    }
    static inline std::set<Permission*> const& getOperatorPermissions() {
        return operatorPermissions;
    }

    enum class DefaultGrantTo {
        ALL, OPERATORS, NONE
    };

    const std::string name;
    const DefaultGrantTo grantTo;
    std::vector<Permission*> children;

    Permission(std::string name, DefaultGrantTo grantTo) : name(name), grantTo(grantTo) {
        permissions[name] = this;
        if (grantTo == DefaultGrantTo::ALL)
            defaultPermissions.insert(this);
        else if (grantTo == DefaultGrantTo::OPERATORS)
            operatorPermissions.insert(this);
    }

    ~Permission() {
        if (grantTo == DefaultGrantTo::ALL)
            defaultPermissions.erase(this);
        else if (grantTo == DefaultGrantTo::OPERATORS)
            operatorPermissions.erase(this);
    }

    void addChild(Permission* perm) {
        children.push_back(perm);
    }

};


