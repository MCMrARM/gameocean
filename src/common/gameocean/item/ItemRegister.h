#pragma once

#include <gameocean/common.h>
#include <map>
#include <memory>

class ItemVariant;
class BlockVariant;

class ItemRegister {

private:
    struct Entry {
        int id = 0;
        ItemVariant *defaultVariant = nullptr;
        std::map<short, ItemVariant *> variants;

        Entry(int id, ItemVariant *def) : id(id), defaultVariant(def) { };
    };

    static std::map<int, std::shared_ptr<Entry>> itemsByIds;
    static std::map<int, std::shared_ptr<Entry>> blocksByIds;
    static std::map<std::string, ItemVariant *> items;
    static std::map<std::string, BlockVariant *> blocks;

public:
    static void registerAssetItems();

    static void registerItemVariant(ItemVariant *variant);
    static void registerBlockVariant(BlockVariant *variant);

    static ItemVariant *getItemVariant(std::string id, bool allowVirtual);
    static BlockVariant *getBlockVariant(std::string id, bool allowVirtual);
    static inline ItemVariant *getItemVariant(std::string id) {
        return getItemVariant(id, false);
    };
    static inline BlockVariant *getBlockVariant(std::string id) {
        return getBlockVariant(id, false);
    };
    static ItemVariant *getItemVariant(int id, int data);
    static BlockVariant *getBlockVariant(int id, int data);

    static ItemVariant *findItem(std::string id);

};


