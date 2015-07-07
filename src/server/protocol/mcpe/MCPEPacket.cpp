#include "MCPEPacket.h"
#include "MCPEPlayer.h"
#include "../../world/World.h"
#include "../../world/Chunk.h"

#include <algorithm>

std::map<int, MCPEPacket::CreatePacket *> MCPEPacket::packets;

void MCPEPacket::registerPackets() {
    MCPEPacket::registerPacket<MCPELoginPacket>(MCPE_LOGIN_PACKET);
}

void MCPEFullChunkDataPacket::write(RakNet::BitStream &stream) {
    stream.Write(chunk->pos.x);
    stream.Write(chunk->pos.z);
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
    MCPEPlayStatusPacket pk;
    pk.status = MCPEPlayStatusPacket::Status::SUCCESS;
    player.writePacket(pk);

    int x = player.getWorld()->spawn.x;
    int y = player.getWorld()->spawn.y;
    int z = player.getWorld()->spawn.z;

    MCPEStartGamePacket pk2;
    pk2.spawnX = x;
    pk2.spawnY = y;
    pk2.spawnZ = z;
    pk2.x = x;
    pk2.y = y;
    pk2.z = z;
    pk2.gamemode = MCPEStartGamePacket::GameMode::CREATIVE;
    player.writePacket(pk2);

    for (int x = -5; x <= 5; x++) {
        for (int z = -5; z <= 5; z++) {
            MCPEFullChunkDataPacket pk3;
            Chunk* sendChunk = player.getWorld()->getChunkAt(x, z);
            if (sendChunk == null) {
                sendChunk = Chunk::empty;
                sendChunk->pos.x = x;
                sendChunk->pos.z = z;
            }
            pk3.chunk = sendChunk;
            player.writePacket(pk3);
        }
    }


    MCPERespawnPacket pk4;
    pk4.x = x;
    pk4.y = y;
    pk4.z = z;
    player.writePacket(pk4);

    MCPEPlayStatusPacket pk5;
    pk5.status = MCPEPlayStatusPacket::Status::PLAYER_SPAWN;
    player.writePacket(pk5);
}