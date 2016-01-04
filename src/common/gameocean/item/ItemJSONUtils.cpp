#include "ItemJSONUtils.h"

#include "json/json.h"
#include "ItemVariant.h"
#include "BlockVariant.h"
#include "ItemRegister.h"
#include "BlockGroup.h"
#include "../model/Model.h"
#include "../utils/ResourceManager.h"

void ItemJSONUtils::parseDataDirectory(std::string path) {
    for (ResourceManager::DirEntry const& file : ResourceManager::instance->getDataDirectoryFiles(path)) {
        if (file.isDir) {
            parseDataDirectory(path + "/" + file.name + "/");
            continue;
        }

        Logger::main->trace("JSON/Item", "Reading data item file: %s", file.name.c_str());
        parseDataFile(path + "/" + file.name);
    }
}

void ItemJSONUtils::parseAssetDirectory(std::string path) {
    for (ResourceManager::DirEntry const& file : ResourceManager::instance->getAssetDirectoryFiles(path)) {
        if (file.isDir) {
            parseAssetDirectory(path + "/" + file.name + "/");
            continue;
        }

        Logger::main->trace("JSON/Item", "Reading asset item file: %s", file.name.c_str());
        parseAssetFile(path + "/" + file.name);
    }
}

void ItemJSONUtils::parseDataFile(std::string filePath) {
    parseStream(*ResourceManager::instance->openDataFile(filePath, std::ios_base::in));
}

void ItemJSONUtils::parseAssetFile(std::string filePath) {
    parseStream(*ResourceManager::instance->openAssetFile(filePath, std::ios_base::in));
}

void ItemJSONUtils::parseStream(std::istream& data) {
    Json::Value val;
    data >> val;
    std::string type = val.get("type", "unknown").asString();
    if (type == "item" || type == "block") {
        int id = val.get("id", 0).asInt();
        std::string nameId = val.get("name_id", "").asString();
        if (nameId.length() <= 0) {
            Logger::main->error("JSON/ItemVariant", "Cannot create item id %i: has no name id", id);
            return;
        }

        const Json::Value& variants = val["variants"];

        if (type == "item") {
            ItemVariant* item = new ItemVariant(id, -1, nameId);
            parseItemVariant(item, val);
            ItemRegister::registerItemVariant(item);

            for (Json::ValueIterator it = variants.begin(); it != variants.end(); it++) {
                int varId = it->get("id", id).asInt();
                int varData = it->get("data", -1).asInt();
                std::string varNameId = it->get("name_id", nameId).asString();
                ItemVariant* variant = new ItemVariant(varId, (short) varData, varNameId);
                variant->copyItemProperties(*item);
                parseItemVariant(variant, *it);
                ItemRegister::registerItemVariant(variant);
            }
        } else if (type == "block") {
            BlockVariant* item = new BlockVariant(id, -1, nameId);
            parseBlockVariant(item, val);
            ItemRegister::registerBlockVariant(item);

            for (Json::ValueIterator it = variants.begin(); it != variants.end(); it++) {
                int varId = it->get("id", id).asInt();
                int varData = it->get("data", -1).asInt();
                std::string varNameId = it->get("name_id", nameId).asString();
                BlockVariant* variant = new BlockVariant(varId, (short) varData, varNameId);
                variant->copyItemProperties(*item);
                variant->copyBlockProperties(*item);
                parseBlockVariant(variant, *it);
                ItemRegister::registerBlockVariant(variant);
            }
        }
    } else if (type == "model") {
        parseModel(val);
    } else {
        Logger::main->error("JSON/Parse", "Invalid data. Type '%s' is invalid.", type.c_str());
    }
}

void ItemJSONUtils::parseItemVariant(ItemVariant* item, Json::Value& val) {
    /*
    std::string base = val.get("base", "").asString();
    if (base.length() > 0) {
        ItemVariant* baseItem = ItemRegister::getItemVariant(base, true);
        if (baseItem == null) {
            Logger::main->error("JSON/ItemVariant", "Base item (%s) not found for %s: is the file order correct?", base.c_str(), item->getNameId().c_str());
            return;
        }
        item->copyItemProperties(*baseItem);
    }
    */

    item->setMaxStackSize(val.get("stack_size", item->getMaxStackSize()).asInt());
    const Json::Value& multiplierAffects = val["multiplier_affects"];
    for (Json::ValueIterator it = multiplierAffects.begin(); it != multiplierAffects.end(); it++) {
        item->toolAffects.insert(BlockGroup::get(it->asString()));
    }
    item->toolBreakMultiplier = val.get("destroy_multiplier", item->toolBreakMultiplier).asFloat();

    const Json::Value& actions = val["actions"];
    if (!actions.empty()) {
        std::string useAction = actions.get("use", "").asString();
        if (useAction.length() > 0 && UseItemAction::handlers.count(useAction) > 0)
            item->useAction = UseItemAction::handlers[useAction];
    }
}

void ItemJSONUtils::parseBlockVariant(BlockVariant* item, Json::Value& val) {
    parseItemVariant(item, val);

    /*
    std::string base = val.get("base", "").asString();
    if (base.length() > 0) {
        BlockVariant* baseItem = ItemRegister::getBlockVariant(base, true);
        if (baseItem == null) {
            Logger::main->error("JSON/BlockVariant", "Base item (%s) not found for %s: is the file order correct?", base.c_str(), item->getNameId().c_str());
            return;
        }
        item->copyBlockProperties(*baseItem);
    }
    */
    item->replaceable = val.get("replaceable", item->replaceable).asBool();
    item->hardness = val.get("hardness", item->hardness).asFloat();
    {
        std::string blockGroup = val.get("group", "").asString();
        if (blockGroup.length() > 0) {
            item->blockGroup = BlockGroup::get(blockGroup);
        }
    }
    item->needsTool = val.get("needs_tool", item->needsTool).asBool();

    if (item->model == nullptr || !val["model"].isNull())
        item->model = Model::getModel(val.get("model", "default").asString());

    const Json::Value& actions = val["actions"];
    if (!actions.empty()) {
        std::string useOnAction = actions.get("use_on", "").asString();
        if (useOnAction.length() > 0 && UseItemAction::handlers.count(useOnAction) > 0)
            item->useOnAction = UseItemAction::handlers[useOnAction];
        std::string destroyAction = actions.get("destroy", "").asString();
        if (destroyAction.length() > 0 && DestroyBlockAction::handlers.count(destroyAction) > 0)
            item->destroyAction = DestroyBlockAction::handlers[destroyAction];
    }
}

void ItemJSONUtils::parseModel(Json::Value& val) {
    std::string nameId = val.get("name_id", "").asString();
    if (nameId.length() <= 0) {
        Logger::main->error("JSON/Model", "Cannot create model: has no name id");
        return;
    }

    Model* model = Model::getModel(nameId);

    const Json::Value& aabbs = val["aabbs"];
    for (Json::ValueIterator it = aabbs.begin(); it != aabbs.end(); it++) {
        if (it->isArray() && it->size() == 6) {
            model->aabbs.push_back({(*it)[0].asFloat(), (*it)[1].asFloat(), (*it)[2].asFloat(),
                                    (*it)[3].asFloat(), (*it)[4].asFloat(), (*it)[5].asFloat()});
        }
    }
}