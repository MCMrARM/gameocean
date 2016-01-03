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
    BlockVariant* variant = action.getTargetBlockVariant();
    BlockPos pos = action.getTargetBlockPos();
    pos = pos.side(action.getTargetBlockSide());
    if (variant != null && variant->replaceable) {
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