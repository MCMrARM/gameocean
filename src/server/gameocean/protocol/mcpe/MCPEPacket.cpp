#include "MCPEPacket.h"

#include <algorithm>
#include <gameocean/utils/Time.h>
#include "MCPEPlayer.h"
#include <gameocean/world/BlockPos.h>
#include <gameocean/world/Chunk.h>
#include <gameocean/world/World.h>
#include <gameocean/world/tile/Tile.h>
#include <gameocean/world/tile/Chest.h>
#include <gameocean/item/recipes/Recipe.h>
#include "../../utils/NBT.h"

MCPEPacket::CreatePacket *MCPEPacket::packets[256];

void MCPEPacket::registerPackets() {
    MCPEPacket::registerPacket<MCPELoginPacket>(MCPE_LOGIN_PACKET);
    MCPEPacket::registerPacket<MCPETextPacket>(MCPE_TEXT_PACKET);
    MCPEPacket::registerPacket<MCPEMovePlayerPacket>(MCPE_MOVE_PLAYER_PACKET);
    MCPEPacket::registerPacket<MCPERemoveBlockPacket>(MCPE_REMOVE_BLOCK_PACKET);
    MCPEPacket::registerPacket<MCPEEntityEventPacket>(MCPE_ENTITY_EVENT_PACKET);
    MCPEPacket::registerPacket<MCPEMobEquipmentPacket>(MCPE_MOB_EQUIPMENT_PACKET);
    MCPEPacket::registerPacket<MCPEMobArmorEquipmentPacket>(MCPE_MOB_ARMOR_EQUIPMENT_PACKET);
    MCPEPacket::registerPacket<MCPEInteractPacket>(MCPE_INTERACT_PACKET);
    MCPEPacket::registerPacket<MCPEUseItemPacket>(MCPE_USE_ITEM_PACKET);
    MCPEPacket::registerPacket<MCPEPlayerActionPacket>(MCPE_PLAYER_ACTION_PACKET);
    MCPEPacket::registerPacket<MCPEContainerClosePacket>(MCPE_CONTAINER_CLOSE_PACKET);
    MCPEPacket::registerPacket<MCPEContainerSetSlotPacket>(MCPE_CONTAINER_SET_SLOT_PACKET);
    MCPEPacket::registerPacket<MCPEContainerSetContentPacket>(MCPE_CONTAINER_SET_CONTENT_PACKET);
    MCPEPacket::registerPacket<MCPECraftingEventPacket>(MCPE_CRAFTING_EVENT_PACKET);
    MCPEPacket::registerPacket<MCPERequestChunkRadiusPacket>(MCPE_REQUEST_CHUNK_RADIUS_PACKET);
}

void MCPETileEntityDataPacket::write(BinaryStream& stream) {
    stream << tile->getPos().x << tile->getPos().y << tile->getPos().z;
    writeTile(stream, *tile);
}

void MCPETileEntityDataPacket::writeTile(BinaryStream& stream, Tile& tile) {
    NBTCompound compound("");
    compound.val["id"] = std::unique_ptr<NBTTag>(new NBTString("id", tile.getId()));
    compound.val["x"] = std::unique_ptr<NBTTag>(new NBTInt("x", tile.getPos().x));
    compound.val["y"] = std::unique_ptr<NBTTag>(new NBTInt("y", tile.getPos().y));
    compound.val["z"] = std::unique_ptr<NBTTag>(new NBTInt("z", tile.getPos().z));
    NBTTag::writeTag(stream, compound, true);
}

void MCPEFullChunkDataPacket::write(BinaryStream& stream) {
    stream << chunk->pos.x << chunk->pos.z;
    stream << (byte) 1; // order type

    DynamicMemoryBinaryStream binaryStream;
    binaryStream << (int) 0;
    chunk->tilesMutex.lock();
    for (std::shared_ptr<Tile> tile : chunk->tiles) {
        MCPETileEntityDataPacket::writeTile(binaryStream, *tile);
    }
    chunk->tilesMutex.unlock();

    int dataSize = sizeof(chunk->blockId) + sizeof(chunk->blockMeta.array) + sizeof(chunk->blockSkylight.array) +
                   sizeof(chunk->blockLight.array) + sizeof(chunk->heightmap) + sizeof(chunk->biomeColors) +
                   binaryStream.getSize();
    stream << dataSize;
    stream.write(chunk->blockId, sizeof(chunk->blockId));
    stream.write(chunk->blockMeta.array, sizeof(chunk->blockMeta.array));
    stream.write(chunk->blockSkylight.array, sizeof(chunk->blockSkylight.array));
    stream.write(chunk->blockLight.array, sizeof(chunk->blockLight.array));
    stream.write(chunk->heightmap, sizeof(chunk->heightmap));
    stream.write((byte*) chunk->biomeColors, sizeof(chunk->biomeColors));
    stream.write(binaryStream.getBuffer(false), binaryStream.getSize());
}

void MCPELoginPacket::handle(MCPEPlayer& player) {
    player.setName(std::string(username));

    player.skinModel = skinModel;
    player.skin = skin;

    std::unique_ptr<MCPEPlayStatusPacket> pk(new MCPEPlayStatusPacket());
    pk->status = MCPEPlayStatusPacket::Status::SUCCESS;
    player.writePacket(std::move(pk));

    int x = player.getWorld().spawn.x;
    int y = player.getWorld().spawn.y;
    int z = player.getWorld().spawn.z;
    player.setPos(x, y, z);

    std::unique_ptr<MCPEStartGamePacket> pk2(new MCPEStartGamePacket());
    pk2->eid = 0;
    pk2->spawnX = x;
    pk2->spawnY = y;
    pk2->spawnZ = z;
    pk2->x = x;
    pk2->y = y;
    pk2->z = z;
    pk2->gamemode = MCPEStartGamePacket::GameMode::SURVIVAL;
    player.writePacket(std::move(pk2));

    std::unique_ptr<MCPEAdventureSettingsPacket> pk3(new MCPEAdventureSettingsPacket());
    player.writePacket(std::move(pk3));

    std::unique_ptr<MCPECraftingDataPacket> pk4(new MCPECraftingDataPacket());
    pk4->recipes = Recipe::recipes;
    player.writePacket(std::move(pk4));

    player.sendWorldTime(player.getWorld().getTime(), player.getWorld().isTimeStopped());
}

void MCPETextPacket::handle(MCPEPlayer& player) {
    player.processMessage(std::string(message));
}

void MCPEMovePlayerPacket::handle(MCPEPlayer& player) {
    player.setRot(yaw, pitch);
    float y = roundf((this->y - player.getHeadY()) * 100.f) / 100.f;
    if (!player.tryMove(x, y, z)) {
        std::unique_ptr<MCPEMovePlayerPacket> pk(new MCPEMovePlayerPacket());
        pk->eid = 0;
        Vector3D v = player.getHeadPos();
        Vector2D r = player.getRot();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->yaw = pk->headYaw = r.x;
        pk->pitch = r.y;
        pk->mode = MCPEMovePlayerPacket::Mode::NORMAL;
        player.writePacket(std::move(pk));
    }
}

void MCPEUpdateBlockPacket::add(World& world, int x, int y, int z, byte flags) {
    WorldBlock b = world.getBlock(x, y, z);
    entries.push_back({x, z, (byte) y, b.id, b.data, flags});
}

const char* MCPEUpdateAttributesPacket::ATTRIBUTE_HEALTH = "generic.health";
const char* MCPEUpdateAttributesPacket::ATTRIBUTE_HUNGER = "player.hunger";
const char* MCPEUpdateAttributesPacket::ATTRIBUTE_EXPERIENCE = "player.experience";
const char* MCPEUpdateAttributesPacket::ATTRIBUTE_EXPERIENCE_LEVEL = "player.level";

void MCPEMobEquipmentPacket::handle(MCPEPlayer& player) {
    if (hotbarSlot < 0 || hotbarSlot >= 9) {
        return;
    }
    int slot = this->slot;
    if (slot == 0x28 || slot == 0 || slot == 255) {
        slot = -1;
    } else {
        slot -= 9;
    }
    if (slot < -1 || slot >= player.inventory.getNumSlots()) {
        return;
    }

    if (slot >= 0 && player.inventory.getItem(slot) != item) {
        ItemInstance itm = player.inventory.getItem(slot);
        player.sendInventory();
        return;
    }

    player.linkHeldItem(hotbarSlot, slot);
}

void MCPEInteractPacket::handle(MCPEPlayer& player) {
    if (actionId == 2) {
        long long n = Time::now();
        if (n - player.lastAttack < 500)
            return;
        player.lastAttack = n;

        for (std::shared_ptr<Entity> ent : player.getNearbyEntities(6.f)) {
            if (ent->getId() == target) {
                player.attack(*ent);
                return;
            }
        }
    }
}

void MCPEUseItemPacket::handle(MCPEPlayer& player) {
    if (player.inventory.getHeldItem() != item) {
        player.sendInventory();

        if (side != 0xff)
            return;
        std::unique_ptr<MCPEUpdateBlockPacket> pk(new MCPEUpdateBlockPacket());
        BlockPos pos = {x, y, z};
        pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        pos = pos.side((BlockPos::Side) side);
        pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        player.writePacket(std::move(pk));
        return;
    }

    if ((y < 0 || y > 127) && side != 0xff)
        return;

    BlockVariant* variant = nullptr;
    if (side == 0xff) {
        UseItemAction action(std::static_pointer_cast<Player>(player.shared_from_this()), item.getItem(), player.getWorld(),
                             nullptr, {x, y, z}, (BlockPos::Side) side, {fx, fy, fz});
        if (!item.isEmpty())
            item.getItem()->use(action);
        return;
    }

    UseItemAction action(std::static_pointer_cast<Player>(player.shared_from_this()), item.getItem(), player.getWorld(),
                         player.getWorld().getBlock({x, y, z}).getBlockVariant(), {x, y, z}, (BlockPos::Side) side, {fx, fy, fz});
    if (!item.isEmpty())
        item.getItem()->use(action);
    else if (action.getTargetBlockVariant() != nullptr)
        action.getTargetBlockVariant()->useOn(action);
    std::unique_ptr<MCPEUpdateBlockPacket> pk(new MCPEUpdateBlockPacket());
    BlockPos pos = {x, y, z};
    pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
    pos = pos.side((BlockPos::Side) side);
    pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
    player.writePacket(std::move(pk));
}

void MCPEPlayerActionPacket::handle(MCPEPlayer& player) {
    if (action == Action::START_BREAK) {
        if (y < 0 || y > 127)
            return;
        player.startMining({x, y, z});
    } else if (action == Action::ABORT_BREAK) {
        player.cancelMining();
    } else if (action == Action::RESPAWN) {
        player.respawn();
    }
}

void MCPEContainerClosePacket::handle(MCPEPlayer& player) {
    player.closeContainer();
}

void MCPEContainerSetSlotPacket::handle(MCPEPlayer& player) {
    if (window == 0 && item == player.inventory.getItem(slot))
        return;
    if (window == 0) {
        player.addTransaction(player.inventory, InventoryTransaction::InventoryKind::PLAYER, slot, item);
    } else if (window == 2) {
        player.addTransaction(player.getOpenedContainer()->getInventory(),
                              InventoryTransaction::InventoryKind::CONTAINER, slot, item);
    } else if (window == 120) {
        player.addTransaction(player.inventory, InventoryTransaction::InventoryKind::ARMOR, slot, item);
    }
}

void MCPECraftingDataPacket::write(BinaryStream& stream) {
    stream << (unsigned int) recipes.size();
    for (auto const& e : recipes) {
        Recipe* recipe = e.second;
        UUID uuid = {e.first, recipe->id};
        if (recipe->isShaped()) {
            stream << (int) 1;

            DynamicMemoryBinaryStream memStream;
            memStream.swapEndian = true;
            ShapedRecipe* shaped = (ShapedRecipe*) recipe;
            memStream << shaped->sizeX << shaped->sizeY;
            const ItemInstance* ingredients = shaped->getIngredients();
            for (int i = 0; i < shaped->sizeX * shaped->sizeY; i++) {
                writeItemInstance(memStream, ingredients[i]);
            }
            memStream << (int) shaped->result.size();
            for (ItemInstance const& itm : shaped->result) {
                writeItemInstance(memStream, itm);
            }
            writeUUID(memStream, uuid);

            stream << memStream.getSize();
            stream.write(memStream.getBuffer(false), (unsigned int) memStream.getSize());
        } else {
            stream << (int) 0;

            DynamicMemoryBinaryStream memStream;
            memStream.swapEndian = true;
            ShapelessRecipe* shapeless = (ShapelessRecipe*) recipe;
            memStream << (int) shapeless->getIngredients().size();
            for (ItemInstance const& itm : shapeless->getIngredients()) {
                writeItemInstance(memStream, itm);
            }
            memStream << (int) shapeless->result.size();
            for (ItemInstance const& itm : shapeless->result) {
                writeItemInstance(memStream, itm);
            }
            writeUUID(memStream, uuid);

            stream << memStream.getSize();
            stream.write(memStream.getBuffer(false), (unsigned int) memStream.getSize());
        }
    }
    writeBool(stream, clearRecipes);
}

void MCPECraftingEventPacket::handle(MCPEPlayer& player) {
    if (window != 0 || Recipe::recipes.count((int) uuid.part1) <= 0) {
        player.sendInventory();
        return;
    }
    Recipe* r = Recipe::recipes[(int) uuid.part1];
    for (auto const& pair : r->summedIngredients) {
        ItemInstance const& itm = pair.second;
        if (!player.inventory.findItem(itm, false)) {
            player.sendInventory();
            return;
        }
    }
    for (auto const& pair : r->summedIngredients) {
        ItemInstance const& itm = pair.second;
        player.inventory.removeItem(itm);
    }
    for (ItemInstance const& i : r->result) {
        player.inventory.addItem(i);
    }
}

void MCPERemoveBlockPacket::handle(MCPEPlayer& player) {
    std::unique_ptr<MCPEUpdateBlockPacket> pk(new MCPEUpdateBlockPacket());
    pk->add(player.getWorld(), x, y, z, MCPEUpdateBlockPacket::FLAG_ALL);
    player.writePacket(std::move(pk));
}

void MCPEEntityEventPacket::handle(MCPEPlayer& player) {
    if (eid == 0 && event == Event::USE_ITEM) {
        // eating
        ItemVariant* item = player.inventory.getHeldItem().getItem();
        if (item != nullptr && item->isFood) {
            player.restoreHunger(item->restoreFoodPoints, item->restoreFoodSaturation);
            int slot = player.inventory.getHeldSlot();
            ItemInstance instance = player.inventory.getItem(slot);
            instance.count--;
            if (instance.count <= 0) {
                instance.setEmpty();
            }
            player.inventory.setItem(slot, instance);
        }
    }
}

void MCPERequestChunkRadiusPacket::handle(MCPEPlayer& player) {
    if (radius < 0 || radius > 32)
        Logger::main->trace("MCPE/RequestChunkRadiusPacket", "Requested chunk radius is %i (seems invalid)", radius);

    int radius = this->radius * this->radius;
    if (radius >= player.server.maxChunkSendCount)
        radius = player.server.maxChunkSendCount;

    player.chunkArrayMutex.lock();
    player.viewChunks = radius;
    player.chunkArrayMutex.unlock();

    std::unique_ptr<MCPEChunkRadiusUpdatePacket> pk(new MCPEChunkRadiusUpdatePacket());
    pk->radius = (int) sqrt(radius);
    Logger::main->trace("MCPE/RequestChunkRadiusPacket", "Requested %i; using %i blocks distance (that's %i chunks)",
                        this->radius, radius, pk->radius);
    player.writePacket(std::move(pk));
}