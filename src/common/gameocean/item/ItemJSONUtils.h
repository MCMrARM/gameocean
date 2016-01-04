#pragma once

#include <istream>

class ItemVariant;
class BlockVariant;
namespace Json {
    class Value;
};
class ItemJSONUtils {

private:
    static void parseModel(Json::Value& val);
    static void parseItemVariant(ItemVariant* item, Json::Value& val);
    static void parseBlockVariant(BlockVariant* item, Json::Value& val);

public:
    static void parseAssetDirectory(std::string path);
    static void parseDataDirectory(std::string path);
    static void parseAssetFile(std::string filePath);
    static void parseDataFile(std::string filePath);
    static void parseStream(std::istream& data);

};


