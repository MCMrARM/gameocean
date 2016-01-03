#include <gameocean/utils/StringUtils.h>
#include "ItemVariant.h"
#include "BlockVariant.h"
#include "../world/World.h"

ItemVariant::ItemVariant(int id, short variantDataId, std::string stringId) : id(id), variantDataId(variantDataId), stringId(stringId) {
    //
}

std::string ItemVariant::getName() {
    return getNameId();
}

bool ItemVariant::use(UseItemAction& action) {
    if (useAction != null) {
        if ((*useAction)(action))
            return true;
    }
    if (action.isUsedOnAir())
        return false;

    return action.getTargetBlockVariant()->useOn(action);
}