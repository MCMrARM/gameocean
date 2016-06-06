#include "ItemRegister.h"

#include "JSONItemLoader.h"
#include "ItemVariant.h"
#include "BlockVariant.h"

#include "../utils/StringUtils.h"

std::map<std::string, ItemVariant *> ItemRegister::items = std::map<std::string, ItemVariant *>();
std::map<std::string, BlockVariant *> ItemRegister::blocks = std::map<std::string, BlockVariant *>();
std::map<int, std::shared_ptr<ItemRegister::Entry>> ItemRegister::itemsByIds = std::map<int, std::shared_ptr<ItemRegister::Entry>>();
std::map<int, std::shared_ptr<ItemRegister::Entry>> ItemRegister::blocksByIds = std::map<int, std::shared_ptr<ItemRegister::Entry>>();

void ItemRegister::registerAssetItems() {
    JSONItemLoader loader;
    loader.parseAssetDirectory("items");
    loader.parseAssetDirectory("blocks");
    loader.registerRecipes();
    loader.callActionLoadCallbacks();
}

void ItemRegister::registerItemVariant(ItemVariant *variant) {
    items[variant->getStringId()] = variant;
    int id = variant->getId();
    if (itemsByIds.count(id) <= 0) {
        itemsByIds[id] = std::shared_ptr<Entry> (new Entry(id, variant));
    }
    if (variant->getVariantDataId() >= 0)
        itemsByIds.at(id)->variants[variant->getVariantDataId()] = variant;
}

void ItemRegister::registerBlockVariant(BlockVariant *variant) {
    ItemRegister::registerItemVariant(variant);

    blocks[variant->getStringId()] = variant;
    int id = variant->getId();
    if (blocksByIds.count(id) <= 0) {
        blocksByIds[id] = std::shared_ptr<Entry> (new Entry(id, variant));
    }
    if (variant->getVariantDataId() >= 0)
        blocksByIds.at(id)->variants[variant->getVariantDataId()] = variant;
}

ItemVariant *ItemRegister::getItemVariant(int id, int data) {
    if (itemsByIds.count(id) <= 0) {
        return nullptr;
    }
    std::shared_ptr<Entry> e = itemsByIds.at(id);
    if (e->variants.count((short) data) <= 0)
        return e->defaultVariant;
    return e->variants.at((short) data);
};

BlockVariant *ItemRegister::getBlockVariant(int id, int data) {
    if (blocksByIds.count(id) <= 0) {
        return nullptr;
    }
    std::shared_ptr<Entry> e = blocksByIds.at(id);
    if (e->variants.count((short) data) <= 0)
        return (BlockVariant *) e->defaultVariant;
    return (BlockVariant *) e->variants[data];
}

ItemVariant *ItemRegister::findItem(std::string id) {
    short data = 0;
    {
        size_t i = id.find(":");
        if (i != std::string::npos) {
            std::string dataStr = id.substr(i + 1);
            id = id.substr(0, i);
            int dataI = StringUtils::asInt(dataStr, -1);
            if (dataI >= 0 && dataI <= UINT16_MAX)
                data = (short) dataI;
        }
    }

    int iid = StringUtils::asInt(id, -1);
    if (iid > 0 && iid < 512) {
        ItemVariant *ret = ItemRegister::getItemVariant(iid, data);
        if (ret != nullptr)
            return ret;
    }

    int m = 0;
    int ml = -1;
    ItemVariant *ret = nullptr;
    for (auto const &e : ItemRegister::items) {
        if (e.second->getId() < 0)
            continue;
        int i = StringUtils::compareLowercase(e.first, id);
        if (i > m || (i == m && e.first.length() < ml)) {
            ret = e.second;
            m = i;
            ml = (int) e.first.length();
        }
    }
    return ret;
}

ItemVariant *ItemRegister::getItemVariant(std::string id, bool allowVirtual) {
    if (items.count(id) <= 0)
        return nullptr;
    ItemVariant *ret = items.at(id);
    if (!allowVirtual && ret->getId() < 0)
        return nullptr;
    return ret;
}
BlockVariant *ItemRegister::getBlockVariant(std::string id, bool allowVirtual) {
    if (blocks.count(id) <= 0)
        return nullptr;
    BlockVariant *ret = blocks.at(id);
    if (!allowVirtual && ret->getId() < 0)
        return nullptr;
    return ret;
}