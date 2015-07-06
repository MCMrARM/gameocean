#include "MCPEPacket.h"
#include "MCPEPlayer.h"

#include <algorithm>

std::map<int, MCPEPacket::CreatePacket *> MCPEPacket::packets;

void MCPEPacket::registerPackets() {
    MCPEPacket::registerPacket<MCPELoginPacket>(MCPE_LOGIN_PACKET);
}

void MCPELoginPacket::handle(MCPEPlayer &player) {
    MCPEPlayStatusPacket pk;
    pk.status = MCPEPlayStatusPacket::Status::SUCCESS;
    player.writePacket(pk);

    MCPEStartGamePacket pk2;
    pk2.spawnX = 0;
    pk2.spawnY = 10;
    pk2.spawnZ = 0;
    pk2.x = 0.f;
    pk2.y = 10.f;
    pk2.z = 0.f;
    player.writePacket(pk2);

    char* data = new char[16 * 16 * 128 + 16 * 16 * 128 / 2 + 16 * 16 * 128 / 2 + 16 * 16 * 128 / 2 + 16 * 16 + 16 * 16 * 4];
    int off = 0;
    memset(&data[off], 0, 16 * 16 * 128); off += 16 * 16 * 128; // block ids
    memset(&data[off], 0, 16 * 16 * 128 / 2); off += 16 * 16 * 128 / 2; // block meta
    memset(&data[off], 0, 16 * 16 * 128 / 2); off += 16 * 16 * 128 / 2; // block skylight
    memset(&data[off], 0, 16 * 16 * 128 / 2); off += 16 * 16 * 128 / 2; // block light
    memset(&data[off], 0, 16 * 16); off += 16 * 16; // heightmap
    memset(&data[off], 0, 16 * 16 * 4); off += 16 * 16 * 4; // biome colors

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 8; y++) {
                data[x * 128 * 16 + z * 128 + y] = 1;
            }
        }
    }

    for (int x = -5; x <= 5; x++) {
        for (int z = -5; z <= 5; z++) {
            MCPEFullChunkDataPacket pk3;
            pk3.chunkX = x;
            pk3.chunkZ = z;
            pk3.data = data;
            pk3.dataSize = off;
            player.writePacket(pk3);
        }
    }

    MCPERespawnPacket pk4;
    pk4.x = 0.f;
    pk4.y = 10.f;
    pk4.z = 0.f;
    player.writePacket(pk4);

    MCPEPlayStatusPacket pk5;
    pk5.status = MCPEPlayStatusPacket::Status::PLAYER_SPAWN;
    player.writePacket(pk5);

    delete[] data;
}