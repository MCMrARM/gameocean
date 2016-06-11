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
class DynamicModel;

class BlockVariant : public ItemVariant {
public:
    bool replaceable = false; // determines if the block can be replaced as it was air
    float hardness = 100;
    BlockGroup *blockGroup = nullptr;
    bool needsTool = false;
    bool fluid = false;
    Model *model = nullptr; // either model or dynamicModel must be set
    DynamicModel *dynamicModel = nullptr;
    bool dropItself = true;
    std::vector<ItemDrop> drops;

    UseItemAction::Handler useOnAction = nullptr;
    std::unique_ptr<ActionHandlerData> useOnActionData;
    DestroyBlockAction::Handler destroyAction = nullptr;
    std::unique_ptr<ActionHandlerData> destroyActionData;

    BlockVariant(int id, short variantDataId, std::string stringId);

    void copyBlockProperties(BlockVariant const &item) {
        this->replaceable = item.replaceable;
        this->hardness = item.hardness;
        this->blockGroup = item.blockGroup;
        this->needsTool = item.needsTool;
        this->fluid = item.fluid;
        this->model = item.model;
        this->dynamicModel = item.dynamicModel;
        this->dropItself = item.dropItself;
        this->drops = item.drops;
    }

    virtual std::string getNameId() { return std::string("block.") + stringId + ".name"; };

    virtual bool use(UseItemAction &action);
    bool useOn(UseItemAction &action);

    void destroy(DestroyBlockAction &action);
    void dropItems(World &world, BlockPos pos, ItemVariant *heldItem);

};


