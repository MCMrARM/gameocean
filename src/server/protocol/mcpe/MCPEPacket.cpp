#include "MCPEPacket.h"
#include "MCPEPlayer.h"
#include "../../world/World.h"
#include "../../world/Chunk.h"

#include <algorithm>

std::map<int, MCPEPacket::CreatePacket *> MCPEPacket::packets;

void MCPEPacket::registerPackets() {
    MCPEPacket::registerPacket<MCPELoginPacket>(MCPE_LOGIN_PACKET);
    MCPEPacket::registerPacket<MCPETextPacket>(MCPE_TEXT_PACKET);
    MCPEPacket::registerPacket<MCPEMovePlayerPacket>(MCPE_MOVE_PLAYER_PACKET);
    MCPEPacket::registerPacket<MCPEEntityEventPacket>(MCPE_ENTITY_EVENT_PACKET);
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
    pk2->gamemode = MCPEStartGamePacket::GameMode::CREATIVE;
    player.writePacket(std::move(pk2));
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

void MCPEUseItemPacket::handle(MCPEPlayer &player) {
    //
}