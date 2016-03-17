#include "DefaultActions.h"

#include <gameocean/world/World.h>
#include <gameocean/entity/Snowball.h>

bool DefaultActions::throwSnowball(UseItemAction& action) {
    if (action.isUsedOnAir()) {
        Vector2D rot = action.getPlayer()->getRot();
        Vector3D aim;
        aim.x = (float) (-std::sin(rot.x / 180.f * M_PI) * std::cos(rot.y / 180.f * M_PI));
        aim.y = (float) (-sin(rot.y / 180.f * M_PI));
        aim.z = (float) (cos(rot.x / 180.f * M_PI) * cos(rot.y / 180.f * M_PI));
        std::shared_ptr<Snowball> sb (new Snowball(action.getWorld(), action.getPlayer()));
        sb->setMotion(aim);
        Vector3D pos = action.getPlayer()->getHeadPos();
        sb->setPos(pos.x, pos.y, pos.z);
    }
    return true;
}

void DefaultActions::registerActions() {
    ItemAction::registerAction<UseItemAction>("throw_snowball", DefaultActions::throwSnowball);
}