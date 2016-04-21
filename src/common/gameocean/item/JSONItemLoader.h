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

    void processModel(const Json::Value& val);
    void processItemVariant(ItemVariant* item, const Json::Value& val);
    void processBlockVariant(BlockVariant* item, const Json::Value& val);
    void processItemRecipe(ItemVariant* item, const Json::Value& val);

    JSONItemDef getRecipeItem(const Json::Value& val);

public:
    void parseAssetDirectory(std::string path);
    void parseDataDirectory(std::string path);
    void parseAssetFile(std::string filePath);
    void parseDataFile(std::string filePath);
    void parseStream(std::istream& data);
    void process(Json::Value const& val);

    void registerRecipes();

};


