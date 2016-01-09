#pragma once

#include <istream>
#include <vector>
#include <string>
#include "ItemInstance.h"

class ItemVariant;
class BlockVariant;
namespace Json {
    class Value;
};
class JSONItemLoader {

private:
    struct JSONItemDef {
        std::string name;
        int count;

        ItemInstance getItemInstance() const;
    };
    struct JSONRecipe {
        enum class Type {
            SHAPED, SHAPELESS
        };

        Type type;
        ItemInstance output;
        std::vector<JSONItemDef> extraOutput;
        std::vector<JSONItemDef> input;
        int shapedSizeX, shapedSizeY;
    };

    std::vector<JSONRecipe> recipes;

    void parseModel(const Json::Value& val);
    void parseItemVariant(ItemVariant* item, const Json::Value& val);
    void parseBlockVariant(BlockVariant* item, const Json::Value& val);
    void parseItemRecipe(ItemVariant* item, const Json::Value& val);

    JSONItemDef getRecipeItem(const Json::Value& val);

public:
    void parseAssetDirectory(std::string path);
    void parseDataDirectory(std::string path);
    void parseAssetFile(std::string filePath);
    void parseDataFile(std::string filePath);
    void parseStream(std::istream& data);

    void registerRecipes();

};


