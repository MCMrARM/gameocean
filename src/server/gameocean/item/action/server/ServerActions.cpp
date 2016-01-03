#include "ServerActions.h"

#include <gameocean/world/World.h>
#include <gameocean/world/tile/Container.h>
#include <gameocean/item/action/UseItemAction.h>

bool ServerActions::openContainer(UseItemAction& action) {
    std::shared_ptr<Container> tile = std::dynamic_pointer_cast<Container>(action.getWorld().getTile(action.getTargetBlockPos()));
    if (tile)
        action.getPlayer()->openContainer(tile);
    printf("container\n");
    return true;
}

void ServerActions::registerActions() {
    ItemAction::registerAction<UseItemAction>("open_container", ServerActions::openContainer);
}