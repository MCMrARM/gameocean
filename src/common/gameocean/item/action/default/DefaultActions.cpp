#include "DefaultActions.h"

#include "json/json.h"
#include "../../ItemRegister.h"
#include <gameocean/world/World.h>
#include <gameocean/entity/Snowball.h>

bool DefaultActions::throwSnowball(UseItemAction& action, ActionHandlerData*) {
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

bool DefaultActions::placeWithFacing(UseItemAction& action, ActionHandlerData*) {
    // TODO:
}

std::unique_ptr<ActionHandlerData> DefaultActions::processPlaceWithFacing(Json::Value const& value) {
    PlaceWithFacingHandlerData* ret = new PlaceWithFacingHandlerData();
    ret->down = ItemRegister::getBlockVariant(value.get("down", "").asString(), false);
    ret->up = ItemRegister::getBlockVariant(value.get("up", "").asString(), false);
    ret->north = ItemRegister::getBlockVariant(value.get("north", "").asString(), false);
    ret->south = ItemRegister::getBlockVariant(value.get("south", "").asString(), false);
    ret->west = ItemRegister::getBlockVariant(value.get("west", "").asString(), false);
    ret->east = ItemRegister::getBlockVariant(value.get("east", "").asString(), false);
    return std::unique_ptr<ActionHandlerData>(ret);
}

bool DefaultActions::placeHalf(UseItemAction& action, ActionHandlerData* data) {
    PlaceHalfHandlerData* halfData = (PlaceHalfHandlerData*) data;
    if (action.isUsedOnAir())
        return false;
    BlockVariant* variant = action.getWorld().getBlock(action.getTargetBlockPos()).getBlockVariant();
    if (((variant == halfData->down && action.getTargetBlockSide() == BlockPos::Side::UP) ||
            (variant == halfData->up && action.getTargetBlockSide() == BlockPos::Side::DOWN)) &&
            halfData->full != nullptr) {
        action.getWorld().setBlock(action.getTargetBlockPos(), halfData->full);
    } else {
        BlockPos pos = action.getTargetBlockPos().side(action.getTargetBlockSide());
        variant = action.getWorld().getBlock(pos).getBlockVariant();
        if (variant != nullptr && !variant->replaceable)
            return false;
        if (action.getTargetBlockSide() == BlockPos::Side::UP) {
            action.getWorld().setBlock(pos, halfData->down);
        } else if (action.getTargetBlockSide() == BlockPos::Side::DOWN) {
            action.getWorld().setBlock(pos, halfData->up);
        } else {
            if (action.getTouchVector().y >= 0.5f && halfData->up != nullptr) {
                action.getWorld().setBlock(pos, halfData->up);
            } else if (halfData->down != nullptr) {
                action.getWorld().setBlock(pos, halfData->down);
            } else {
                return false;
            }
        }
    }
    action.getPlayer()->inventory.removeItem(ItemInstance(action.getItemVariant()->getId(), 1, action.getItemVariant()->getVariantDataId()));
    return true;
}

std::unique_ptr<ActionHandlerData> DefaultActions::processPlaceHalf(Json::Value const& value) {
    PlaceHalfHandlerData* ret = new PlaceHalfHandlerData();
    ret->down = ItemRegister::getBlockVariant(value.get("down", "").asString(), false);
    ret->up = ItemRegister::getBlockVariant(value.get("up", "").asString(), false);
    ret->full = ItemRegister::getBlockVariant(value.get("full", "").asString(), false);
    return std::unique_ptr<ActionHandlerData>(ret);
}

void DefaultActions::registerActions() {
    ItemAction::registerAction<UseItemAction>("throw_snowball", DefaultActions::throwSnowball);
    ItemAction::registerAction<UseItemAction>("place_facing", DefaultActions::processPlaceWithFacing, DefaultActions::placeWithFacing);
    ItemAction::registerAction<UseItemAction>("place_half", DefaultActions::processPlaceHalf, DefaultActions::placeHalf);
}