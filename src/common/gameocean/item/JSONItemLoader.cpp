#include <gameocean/item/recipes/Recipe.h>
#include "JSONItemLoader.h"

#include "json/json.h"
#include "ItemVariant.h"
#include "BlockVariant.h"
#include "ItemRegister.h"
#include "ItemGroup.h"
#include "BlockGroup.h"
#include "../model/Model.h"
#include "../utils/ResourceManager.h"
#include "recipes/Recipe.h"

void JSONItemLoader::parseDataDirectory(std::string path) {
    for (ResourceManager::DirEntry const& file : ResourceManager::instance->getDataDirectoryFiles(path)) {
        if (file.isDir) {
            parseDataDirectory(path + "/" + file.name + "/");
            continue;
        }

        Logger::main->trace("JSON/Item", "Reading data item file: %s", file.name.c_str());
        parseDataFile(path + "/" + file.name);
    }
}

void JSONItemLoader::parseAssetDirectory(std::string path) {
    for (ResourceManager::DirEntry const& file : ResourceManager::instance->getAssetDirectoryFiles(path)) {
        if (file.isDir) {
            parseAssetDirectory(path + "/" + file.name + "/");
            continue;
        }

        Logger::main->trace("JSON/Item", "Reading asset item file: %s", file.name.c_str());
        parseAssetFile(path + "/" + file.name);
    }
}

void JSONItemLoader::parseDataFile(std::string filePath) {
    parseStream(*ResourceManager::instance->openDataFile(filePath, std::ios_base::in));
}

void JSONItemLoader::parseAssetFile(std::string filePath) {
    parseStream(*ResourceManager::instance->openAssetFile(filePath, std::ios_base::in));
}

void JSONItemLoader::parseStream(std::istream& data) {
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

void JSONItemLoader::parseItemVariant(ItemVariant* item, const Json::Value& val) {
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
    {
        std::string toolGroup = val.get("tool_group", "").asString();
        if (toolGroup.length() > 0) {
            item->toolGroup = ItemGroup::get(toolGroup);
        }
    }
    {
        const Json::Value& multiplierAffects = val["multiplier_affects"];
        for (Json::ValueIterator it = multiplierAffects.begin(); it != multiplierAffects.end(); it++) {
            item->toolAffects.insert(BlockGroup::get(it->asString()));
        }
        item->toolBreakMultiplier = val.get("destroy_multiplier", item->toolBreakMultiplier).asFloat();
    }

    item->attackDamage = val.get("attack_damage", item->attackDamage).asFloat();
    item->damageReduction = val.get("damage_reduction", item->damageReduction).asFloat();
    item->isFood = val.get("is_food", item->isFood).asBool();
    item->restoreFoodPoints = val.get("food_points", item->restoreFoodPoints).asFloat();
    item->restoreFoodSaturation = val.get("food_saturation", item->restoreFoodSaturation).asFloat();

    {
        const Json::Value& actions = val["actions"];
        if (!actions.empty()) {
            std::string useAction = actions.get("use", "").asString();
            if (useAction.length() > 0 && UseItemAction::handlers.count(useAction) > 0)
                item->useAction = UseItemAction::handlers[useAction];
        }
    }
    {
        const Json::Value& recipe = val["recipe"];
        if (!recipe.empty()) {
            parseItemRecipe(item, recipe);
        }
    }
    {
        const Json::Value& recipes = val["recipes"];
        for (const Json::Value& recipe : recipes) {
            parseItemRecipe(item, recipe);
        }
    }
}

void JSONItemLoader::parseBlockVariant(BlockVariant* item, const Json::Value& val) {
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

    {
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
    {
        const Json::Value& drops = val["drops"];
        if (!drops.empty()) {
            for (const Json::Value& drop : drops) {
                ItemDrop itmDrop;
                itmDrop.dropVariantId = drop.get("name_id", item->getNameId()).asString();
                itmDrop.dropCount = drop.get("count", 1).asInt();
                itmDrop.chances = drop.get("chances", 1.f).asFloat();

                const Json::Value& requires = drop["requires"];
                if (!requires.empty()) {
                    itmDrop.requiredVariantId = requires.get("name_id", "").asString();

                    std::string toolGroup = requires.get("group", "").asString();
                    if (toolGroup.length() > 0) {
                        itmDrop.requiredGroup = ItemGroup::get(toolGroup);
                    }
                }

                item->drops.push_back(itmDrop);
            }
        }
        if (item->drops.size() > 0)
            item->dropItself = false;
    }
}

void JSONItemLoader::parseModel(const Json::Value& val) {
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

void JSONItemLoader::parseItemRecipe(ItemVariant* item, const Json::Value& val) {
    std::string type = val.get("type", "shaped").asString();

    int outCount = val.get("count", 1).asInt();

    JSONRecipe recipe;
    recipe.output = ItemInstance (item, (byte) outCount, item->getVariantDataId());
    if (type == "shaped") {
        recipe.type = JSONRecipe::Type::SHAPED;

        const Json::Value& ingredients = val["ingredients"];
        recipe.shapedSizeY = ingredients.size();
        recipe.shapedSizeX = ingredients[0].size();

        for (int y = 0; y < recipe.shapedSizeY; y++) {
            const Json::Value& i = ingredients[y];
            for (int x = 0; x < recipe.shapedSizeX; x++) {
                recipe.input.push_back(getRecipeItem(i[x]));
            }
        }
    } else if (type == "shapeless") {
        recipe.type = JSONRecipe::Type::SHAPELESS;

        const Json::Value& ingredients = val["ingredients"];
        for (const Json::Value& ingredient : ingredients) {
            recipe.input.push_back(getRecipeItem(ingredient));
        }
    }

    const Json::Value& extra_result = val["extra_result"];
    for (const Json::Value& extra : extra_result) {
        recipe.extraOutput.push_back(getRecipeItem(extra));
    }

    recipes.push_back(std::move(recipe));
}

JSONItemLoader::JSONItemDef JSONItemLoader::getRecipeItem(const Json::Value& val) {
    if (val.isArray()) {
        return { val[0].asString(), val.get(1, 1).asInt() };
    } else {
        return { val.asString(), 1 };
    }
}

void JSONItemLoader::registerRecipes() {
    for (JSONRecipe const& recipe : recipes) {
        Recipe* r = nullptr;
        if (recipe.type == JSONRecipe::Type::SHAPED) {
            ShapedRecipe* shaped = new ShapedRecipe(recipe.output, recipe.shapedSizeX, recipe.shapedSizeY);
            r = shaped;

            int x = 0;
            int y = 0;
            for (JSONItemDef const& i : recipe.input) {
                ItemInstance itm = i.getItemInstance();
                if (!itm.isEmpty())
                    shaped->addIngredient(x, y, itm);
                x++;
                if (y >= recipe.shapedSizeX) {
                    y++;
                    x = 0;
                }
            }
        } else if (recipe.type == JSONRecipe::Type::SHAPELESS) {
            ShapelessRecipe* shapeless = new ShapelessRecipe(recipe.output);
            r = shapeless;

            for (JSONItemDef const& i : recipe.input) {
                ItemInstance itm = i.getItemInstance();
                if (!itm.isEmpty())
                    shapeless->addIngredient(itm);
            }
        }

        if (r == nullptr)
            continue;
        for (JSONItemDef const& i : recipe.extraOutput) {
            r->result.push_back(i.getItemInstance());
        }
    }
}

ItemInstance JSONItemLoader::JSONItemDef::getItemInstance() const {
    ItemVariant* v = ItemRegister::getItemVariant(name);
    if (v == nullptr)
        return ItemInstance ();
    return ItemInstance (v, (byte) count, v->getVariantDataId());
}