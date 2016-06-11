#include "BlockVariant.h"
#include "../utils/Random.h"

#ifdef SERVER
#include <gameocean/Player.h>
#include <gameocean/plugin/event/player/PlayerBlockPlaceEvent.h>
#include "../world/World.h"
#endif

BlockVariant::BlockVariant(int id, short variantDataId, std::string stringId) : ItemVariant(id, variantDataId, stringId) {
    //
}

bool BlockVariant::use(UseItemAction &action) {
    if (ItemVariant::use(action))
        return true;
#ifdef SERVER
    if (action.isUsedOnAir())
        return false;
    PlayerBlockPlaceEvent event (*action.getPlayer(), action.getWorld(), this, action.getTargetBlockPos(),
                                 action.getTargetBlockSide());
    Event::broadcast(event);
    if (event.isCancelled())
        return false;
    BlockPos pos = action.getTargetBlockPos().side(action.getTargetBlockSide());
    BlockVariant *variant = action.getWorld().getBlock(pos).getBlockVariant();
    if (variant == nullptr || variant->replaceable) {
        action.getWorld().setBlock(pos, (BlockId) id, (byte) variantDataId);
        action.getPlayer()->inventory.removeItem(ItemInstance(id, 1, variantDataId));
        return true;
    }
#endif
}

bool BlockVariant::useOn(UseItemAction &action) {
    if (useOnAction != nullptr) {
        return (*useOnAction)(action, useOnActionData.get());
    }
    return false;
}

void BlockVariant::destroy(DestroyBlockAction &action) {
    if (destroyAction != nullptr && (*destroyAction)(action, destroyActionData.get())) // if the event was handled, don't do anything here
        return;
#ifdef SERVER
    action.getWorld().setBlock(action.getBlockPos(), 0, 0);
    dropItems(action.getWorld(), action.getBlockPos(), action.getPlayer()->inventory.getHeldItem().getItem());
#endif
}

void BlockVariant::dropItems(World &world, BlockPos pos, ItemVariant *heldItem) {
    Vector3D dropPos (pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f);
    if (dropItself) {
        world.dropItem(dropPos, ItemInstance (this, 1, variantDataId));
    }
    Random &random = Random::instance;
    for (ItemDrop drop : drops) {
        if (drop.requiredGroup != nullptr && (heldItem == nullptr || heldItem->toolGroup != drop.requiredGroup))
            continue;
        if (drop.requiredVariantId.length() > 0 && heldItem->getStringId() != drop.requiredVariantId)
            continue;
        if (drop.chances != 1.f) {
            float r = random.nextFloat();
            if (drop.chances >= r)
                continue;
        }

        ItemVariant *variant = ItemRegister::getItemVariant(drop.dropVariantId);
        if (variant != nullptr)
            world.dropItem(dropPos, ItemInstance (variant, (byte) drop.dropCount, variant->getVariantDataId()));
    }
}