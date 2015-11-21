#include "MCPEPacket.h"

#include <algorithm>
#include <gameocean/utils/Time.h>
#include "MCPEPlayer.h"
#include "../../world/BlockPos.h"
#include "../../world/Chunk.h"
#include "../../world/World.h"

std::map<int, MCPEPacket::CreatePacket *> MCPEPacket::packets;

void MCPEPacket::registerPackets() {
    MCPEPacket::registerPacket<MCPELoginPacket>(MCPE_LOGIN_PACKET);
    MCPEPacket::registerPacket<MCPETextPacket>(MCPE_TEXT_PACKET);
    MCPEPacket::registerPacket<MCPEMovePlayerPacket>(MCPE_MOVE_PLAYER_PACKET);
    MCPEPacket::registerPacket<MCPERemoveBlockPacket>(MCPE_REMOVE_BLOCK_PACKET);
    MCPEPacket::registerPacket<MCPEEntityEventPacket>(MCPE_ENTITY_EVENT_PACKET);
    MCPEPacket::registerPacket<MCPEMobEquipmentPacket>(MCPE_MOB_EQUIPMENT_PACKET);
    MCPEPacket::registerPacket<MCPEInteractPacket>(MCPE_INTERACT_PACKET);
    MCPEPacket::registerPacket<MCPEUseItemPacket>(MCPE_USE_ITEM_PACKET);
    MCPEPacket::registerPacket<MCPEPlayerActionPacket>(MCPE_PLAYER_ACTION_PACKET);
    MCPEPacket::registerPacket<MCPEContainerSetSlotPacket>(MCPE_CONTAINER_SET_SLOT_PACKET);
    MCPEPacket::registerPacket<MCPEContainerSetContentPacket>(MCPE_CONTAINER_SET_CONTENT_PACKET);
}

void MCPEFullChunkDataPacket::write(RakNet::BitStream &stream) {
    stream.Write(chunk->pos.x);
    stream.Write(chunk->pos.z);
    stream.Write((byte) 1); // order type
    int dataSize = sizeof(chunk->blockId) + sizeof(chunk->blockMeta.array) + sizeof(chunk->blockSkylight.array) + sizeof(chunk->blockLight.array) + sizeof(chunk->heightmap) + sizeof(chunk->biomeColors);
    stream.Write(dataSize);
    stream.Write((char*) chunk->blockId, sizeof(chunk->blockId));
    stream.Write((char*) chunk->blockMeta.array, sizeof(chunk->blockMeta.array));
    stream.Write((char*) chunk->blockSkylight.array, sizeof(chunk->blockSkylight.array));
    stream.Write((char*) chunk->blockLight.array, sizeof(chunk->blockLight.array));
    stream.Write((char*) chunk->heightmap, sizeof(chunk->heightmap));
    stream.Write((char*) chunk->biomeColors, sizeof(chunk->biomeColors));
}

void MCPELoginPacket::handle(MCPEPlayer &player) {
    player.setName(std::string(username));

    std::unique_ptr<MCPEPlayStatusPacket> pk (new MCPEPlayStatusPacket());
    pk->status = MCPEPlayStatusPacket::Status::SUCCESS;
    player.writePacket(std::move(pk));

    int x = player.getWorld().spawn.x;
    int y = player.getWorld().spawn.y;
    int z = player.getWorld().spawn.z;
    player.setPos(x, y, z);

    std::unique_ptr<MCPEStartGamePacket> pk2 (new MCPEStartGamePacket());
    pk2->eid = 0;
    pk2->spawnX = x;
    pk2->spawnY = y;
    pk2->spawnZ = z;
    pk2->x = x;
    pk2->y = y;
    pk2->z = z;
    pk2->gamemode = MCPEStartGamePacket::GameMode::SURVIVAL;
    player.writePacket(std::move(pk2));

    player.sendWorldTime(player.getWorld().getTime(), player.getWorld().isTimeStopped());
}

void MCPETextPacket::handle(MCPEPlayer &player) {
    player.processMessage(std::string(message));
}

void MCPEMovePlayerPacket::handle(MCPEPlayer &player) {
    if (!player.tryMove(x, y, z)) {
        std::unique_ptr<MCPEMovePlayerPacket> pk (new MCPEMovePlayerPacket());
        pk->eid = 0;
        Vector3D v = player.getPos();
        pk->x = v.x;
        pk->y = v.y;
        pk->z = v.z;
        pk->mode = MCPEMovePlayerPacket::Mode::NORMAL;
        player.writePacket(std::move(pk));
    }
}

void MCPEUpdateBlockPacket::add(World& world, int x, int y, int z, byte flags) {
    WorldBlock b = world.getBlock(x, y, z);
    entries.push_back({ x, z, (byte) y, b.id, b.data, flags });
}

void MCPEMobEquipmentPacket::handle(MCPEPlayer &player) {
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

        for (Entity* ent : player.getNearbyEntities(6.f)) {
            if (ent->getId() == target) {
                player.attack(*ent);
                return;
            }
        }
    }
}

void MCPEUseItemPacket::handle(MCPEPlayer &player) {
    if (player.inventory.getHeldItem() != item) {
        player.sendInventory();

        std::unique_ptr<MCPEUpdateBlockPacket> pk (new MCPEUpdateBlockPacket());
        BlockPos pos = {x, y, z};
        pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        pos = pos.side((BlockPos::Side) side);
        pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        player.writePacket(std::move(pk));
        return;
    }

    if (y < 0 || y > 127)
        return;

    if (item.getItemId() > 0 && item.getItemId() < 256) {
        std::unique_ptr<MCPEUpdateBlockPacket> pk (new MCPEUpdateBlockPacket());
        BlockPos pos = {x, y, z};
        pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        if (player.getWorld().getBlock(pos).id != 0) {
            pos = pos.side((BlockPos::Side) side);
            WorldBlock wb = player.getWorld().getBlock(pos);
            BlockVariant* bv = wb.getBlockVariant();
            if (wb.id == 0 || (bv != null && bv->replaceable)) {
                player.getWorld().setBlock(pos, item.getItemId(), item.getItemData());
                player.inventory.removeItem(ItemInstance(item.getItem(), 1, item.getItemData()));
            }
            pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        } else {
            pos = pos.side((BlockPos::Side) side);
            pk->add(player.getWorld(), pos.x, pos.y, pos.z, MCPEUpdateBlockPacket::FLAG_ALL);
        }
        player.writePacket(std::move(pk));
    }
}

void MCPEPlayerActionPacket::handle(MCPEPlayer& player) {
    if (action == Action::START_BREAK) {
        if (y < 0 || y > 127)
            return;
        player.startMining({ x, y, z });
    } else if (action == Action::ABORT_BREAK) {
        player.cancelMining();
    }
}

void MCPERemoveBlockPacket::handle(MCPEPlayer& player) {
    std::unique_ptr<MCPEUpdateBlockPacket> pk (new MCPEUpdateBlockPacket());
    pk->add(player.getWorld(), x, y, z, MCPEUpdateBlockPacket::FLAG_ALL);
    player.writePacket(std::move(pk));
}