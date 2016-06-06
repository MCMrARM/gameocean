#include "Permission.h"

std::map<std::string, Permission *> Permission::permissions;
std::set<Permission *> Permission::defaultPermissions;
std::set<Permission *> Permission::operatorPermissions;
Permission *Permission::defaultCommands;
Permission *Permission::defaultPlayerCommands;
Permission *Permission::defaultOperatorCommands;
Permission *Permission::helpCommand;
Permission *Permission::suicideCommand;
Permission *Permission::stopServerCommand;
Permission *Permission::reloadServerCommand;
Permission *Permission::serverVersionCommand;
Permission *Permission::pingCommand;
Permission *Permission::teleportPlayerCommand;
Permission *Permission::giveItemCommand;
Permission *Permission::worldTimeCommand;

Permission *Permission::getPermission(std::string name) {
    if (permissions.count(name) > 0)
        return permissions[name];
    return nullptr;
}

void Permission::initializeDefaultPermissions() {
    defaultCommands = new Permission("commands.default.all", Permission::DefaultGrantTo::ALL);
    defaultPlayerCommands = new Permission("commands.default.player", Permission::DefaultGrantTo::ALL);
    defaultOperatorCommands = new Permission("commands.default.operator", Permission::DefaultGrantTo::OPERATORS);
    helpCommand = new Permission("commands.default.help", Permission::DefaultGrantTo::ALL);
    suicideCommand = new Permission("commands.default.suicide", Permission::DefaultGrantTo::ALL);
    stopServerCommand = new Permission("commands.default.server.stop", Permission::DefaultGrantTo::OPERATORS);
    reloadServerCommand = new Permission("commands.default.server.reload", Permission::DefaultGrantTo::OPERATORS);
    serverVersionCommand = new Permission("commands.default.server.version", Permission::DefaultGrantTo::ALL);
    pingCommand = new Permission("commands.default.ping", Permission::DefaultGrantTo::ALL);
    giveItemCommand = new Permission("commands.default.give", Permission::DefaultGrantTo::OPERATORS);
    worldTimeCommand = new Permission("commands.default.world.time", Permission::DefaultGrantTo::OPERATORS);
    defaultPlayerCommands->children.push_back(helpCommand);
    defaultPlayerCommands->children.push_back(suicideCommand);
    defaultOperatorCommands->children.push_back(stopServerCommand);
    defaultOperatorCommands->children.push_back(reloadServerCommand);
    defaultPlayerCommands->children.push_back(serverVersionCommand);
    defaultPlayerCommands->children.push_back(pingCommand);
    defaultOperatorCommands->children.push_back(giveItemCommand);
    defaultOperatorCommands->children.push_back(worldTimeCommand);
    defaultCommands->children.push_back(defaultPlayerCommands);
    defaultCommands->children.push_back(defaultOperatorCommands);
}