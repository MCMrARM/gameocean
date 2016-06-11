#include "DefaultActions.h"

#include "json/json.h"
#include "../../ItemRegister.h"
#include <gameocean/world/World.h>
#include <gameocean/entity/Snowball.h>
#ifdef SERVER
#include <gameocean/Player.h>
#include <gameocean/plugin/event/player/PlayerBlockPlaceEvent.h>
#endif

bool DefaultActions::throwSnowball(UseItemAction &action, ActionHandlerData *) {
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

bool DefaultActions::placeWithFacing(UseItemAction &action, ActionHandlerData *) {
    // TODO:
}

std::unique_ptr<ActionHandlerData> DefaultActions::processPlaceWithFacing(Json::Value const &value) {
    PlaceWithFacingHandlerData *ret = new PlaceWithFacingHandlerData();
    ret->down = ItemRegister::getBlockVariant(value.get("down", "").asString(), false);
    ret->up = ItemRegister::getBlockVariant(value.get("up", "").asString(), false);
    ret->north = ItemRegister::getBlockVariant(value.get("north", "").asString(), false);
    ret->south = ItemRegister::getBlockVariant(value.get("south", "").asString(), false);
    ret->west = ItemRegister::getBlockVariant(value.get("west", "").asString(), false);
    ret->east = ItemRegister::getBlockVariant(value.get("east", "").asString(), false);
    return std::unique_ptr<ActionHandlerData>(ret);
}

bool DefaultActions::placeHalf(UseItemAction &action, ActionHandlerData *data) {
#ifdef SERVER
    PlaceHalfHandlerData *halfData = (PlaceHalfHandlerData *) data;
    if (action.isUsedOnAir())
        return false;
    BlockVariant *targetVariant = nullptr;
    BlockVariant *variant = action.getWorld().getBlock(action.getTargetBlockPos()).getBlockVariant();
    if (((variant == halfData->down && action.getTargetBlockSide() == BlockPos::Side::UP) ||
            (variant == halfData->up && action.getTargetBlockSide() == BlockPos::Side::DOWN)) &&
            halfData->full != nullptr) {
        targetVariant = halfData->full;
    } else {
        BlockPos pos = action.getTargetBlockPos().side(action.getTargetBlockSide());
        variant = action.getWorld().getBlock(pos).getBlockVariant();
        if (variant != nullptr && !variant->replaceable)
            return false;
        if (action.getTargetBlockSide() == BlockPos::Side::UP) {
            targetVariant = halfData->down;
        } else if (action.getTargetBlockSide() == BlockPos::Side::DOWN) {
            targetVariant = halfData->up;
        } else {
            if (action.getTouchVector().y >= 0.5f && halfData->up != nullptr) {
                targetVariant = halfData->up;
            } else if (halfData->down != nullptr) {
                targetVariant = halfData->down;
            } else {
                return false;
            }
        }
    }
    if (targetVariant == nullptr)
        return false;
    PlayerBlockPlaceEvent event (*action.getPlayer(), action.getWorld(), targetVariant,
                                 action.getTargetBlockPos(), action.getTargetBlockSide());
    Event::broadcast(event);
    if (event.isCancelled())
        return true; // return true so the event won't be called twice
    action.getWorld().setBlock(action.getTargetBlockPos(), targetVariant);
    action.getPlayer()->inventory.removeItem(ItemInstance(action.getItemVariant()->getId(), 1, action.getItemVariant()->getVariantDataId()));
#endif
    return true;
}

std::unique_ptr<ActionHandlerData> DefaultActions::processPlaceHalf(Json::Value const& value) {
    PlaceHalfHandlerData *ret = new PlaceHalfHandlerData();
    ret->down = ItemRegister::getBlockVariant(value.get("down", "").asString(), false);
    ret->up = ItemRegister::getBlockVariant(value.get("up", "").asString(), false);
    ret->full = ItemRegister::getBlockVariant(value.get("full", "").asString(), false);
    return std::unique_ptr<ActionHandlerData>(ret);
}

std::unique_ptr<ActionHandlerData> DefaultActions::processPlaceDoor(Json::Value const& value) {
    PlaceDoorHandlerData *ret = new PlaceDoorHandlerData();
    ret->block = ItemRegister::getBlockVariant(value.get("block", "").asString(), false);
    return std::unique_ptr<ActionHandlerData>(ret);
}

bool DefaultActions::placeDoor(UseItemAction &action, ActionHandlerData *data) {
#ifdef SERVER
    if (action.isUsedOnAir())
        return false;
    BlockVariant *block = ((PlaceDoorHandlerData *) data)->block;
    PlayerBlockPlaceEvent event (*action.getPlayer(), action.getWorld(), block,
                                 action.getTargetBlockPos(), action.getTargetBlockSide());
    Event::broadcast(event);
    if (event.isCancelled())
        return false;
    BlockPos pos = action.getTargetBlockPos().side(action.getTargetBlockSide());
    BlockVariant *variant = action.getWorld().getBlock(pos).getBlockVariant();
    BlockPos pos2 = pos.side(BlockPos::Side::UP);
    BlockVariant *variant2 = action.getWorld().getBlock(pos2).getBlockVariant();
    if ((variant == nullptr || variant->replaceable) && (variant2 == nullptr || variant2->replaceable)) {
        byte rotData = (byte) ((((int) action.getPlayer()->getRot().x % 360 + 360 + 180 - 45) % 360) / 90);
        byte opensRightData = (((int) action.getPlayer()->getRot().x % 90 + 90) % 90) < 45 ? (byte) 1 : (byte) 0;
        action.getWorld().setBlock(pos, (BlockId) block->getId(), rotData);
        action.getWorld().setBlock(pos2, (BlockId) block->getId(), (byte) (opensRightData | 8));
        action.getPlayer()->inventory.removeItem(ItemInstance(action.getItemVariant()->getId(), 1,
                                                              action.getItemVariant()->getVariantDataId()));
        return true;
    }
#endif
    return true;
}

bool DefaultActions::openDoor(UseItemAction &action, ActionHandlerData *) {
    BlockPos pos = action.getTargetBlockPos();
    WorldBlock block = action.getWorld().getBlock(pos);
    if ((block.data & 8) != 0) { // top
        pos = pos.side(BlockPos::Side::DOWN);
        block = action.getWorld().getBlock(pos);
        if (block.getBlockVariant() != action.getTargetBlockVariant())
            return false;
    }
    action.getWorld().setBlock(pos, block.id, (byte) (block.data ^ 4));
    return true;
}

bool DefaultActions::destroyDoor(DestroyBlockAction &action, ActionHandlerData *) {
    BlockPos pos = action.getBlockPos();
    WorldBlock block = action.getWorld().getBlock(pos);
    if ((block.data & 8) != 0) { // top
        pos = pos.side(BlockPos::Side::DOWN);
    } else { // bottom
        pos = pos.side(BlockPos::Side::UP);
    }
    block = action.getWorld().getBlock(pos);
    if (block.getBlockVariant() == action.getBlockVariant())
        action.getWorld().setBlock(pos, 0, 0);
    return false; // returns false so the event won't be considered handled and the block will be destroyed
}

void DefaultActions::registerActions() {
    ItemAction::registerAction<UseItemAction>("throw_snowball", DefaultActions::throwSnowball);
    ItemAction::registerAction<UseItemAction>("place_facing", DefaultActions::processPlaceWithFacing, DefaultActions::placeWithFacing);
    ItemAction::registerAction<UseItemAction>("place_half", DefaultActions::processPlaceHalf, DefaultActions::placeHalf);
    ItemAction::registerAction<UseItemAction>("place_door", DefaultActions::processPlaceDoor, DefaultActions::placeDoor);
    ItemAction::registerAction<UseItemAction>("open_door", DefaultActions::openDoor);
    ItemAction::registerAction<DestroyBlockAction>("destroy_door", DefaultActions::destroyDoor);
}