#include <utils/StringUtils.h>
#include "ItemVariant.h"

ItemVariant::ItemVariant(int id, short variantDataId, std::string stringId) : id(id), variantDataId(variantDataId), stringId(stringId) {
    //
}

std::string ItemVariant::getName() {
    return getNameId();
}