#include "BlockVariant.h"

#ifdef SERVER
#include <gameocean/Player.h>
#include "../world/World.h"
#endif

BlockVariant::BlockVariant(int id, short variantDataId, std::string stringId) : ItemVariant(id, variantDataId, stringId) {
    //
}

bool BlockVariant::use(UseItemAction& action) {
    if (ItemVariant::use(action))
        return true;
#ifdef SERVER
    if (action.isUsedOnAir())
        return false;
    BlockPos pos = action.getTargetBlockPos().side(action.getTargetBlockSide());
    BlockVariant* variant = action.getWorld().getBlock(pos).getBlockVariant();
    if (variant == null || variant->replaceable) {
        action.getWorld().setBlock(pos, (BlockId) id, (byte) variantDataId);
        action.getPlayer()->inventory.removeItem(ItemInstance(id, 1, variantDataId));
    }
#endif
}

bool BlockVariant::useOn(UseItemAction& action) {
    if (useOnAction != null) {
        return (*useOnAction)(action);
    }
    return false;
}