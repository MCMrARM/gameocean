#pragma once

#include <string>
#include <map>
#include <gameocean/common.h>
#include "action/DestroyBlockAction.h"

#include "ItemVariant.h"
#include "ItemDrop.h"

typedef byte BlockId;
class BlockGroup;
class Model;

class BlockVariant : public ItemVariant {
public:
    bool replaceable = false; // determines if the block can be replaced as it was air
    float hardness = 100;
    BlockGroup* blockGroup = null;
    bool needsTool = false;
    Model* model = null;
    bool dropItself = true;
    std::vector<ItemDrop> drops;

    UseItemAction::Handler useOnAction = nullptr;
    DestroyBlockAction::Handler destroyAction = nullptr;

    BlockVariant(int id, short variantDataId, std::string stringId);

    void copyBlockProperties(BlockVariant const& item) {
        this->replaceable = item.replaceable;
        this->hardness = item.hardness;
        this->blockGroup = item.blockGroup;
        this->needsTool = item.needsTool;
        this->model = item.model;
        this->drops = item.drops;
    };

    virtual std::string getNameId() { return std::string("block.") + stringId + ".name"; };

    virtual bool use(UseItemAction& action);
    bool useOn(UseItemAction& action);

    void dropItems(World& world, BlockPos pos, ItemVariant* heldItem);

};


