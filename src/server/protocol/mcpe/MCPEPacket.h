#pragma once

#include <map>
#include "common.h"
#include "../../libs/RakNet/BitStream.h"

enum class MCPEChannel : unsigned char {
    NONE, PRIORITY, WORLD_CHUNKS, MOVEMENT, BLOCKS, WORLD_EVENTS, ENTITY_SPAWNING, TEXT
};

enum MCPEMessages {
    MCPE_LOGIN_PACKET = 0x82,
    MCPE_PLAY_STATUS_PACKET = 0x83,
    MCPE_DISCONNECT_PACKET = 0x84,
    MCPE_TEXT_PACKET = 0x85,
    MCPE_SET_TIME_PACKET = 0x86,
    MCPE_START_GAME_PACKET = 0x87,
    MCPE_ADD_PLAYER_PACKET = 0x88,
    MCPE_REMOVE_PLAYER_PACKET = 0x89,
    MCPE_ADD_ENTITY_PACKET = 0x8a,
    MCPE_REMOVE_ENTITY_PACKET = 0x8b,
    MCPE_ADD_ITEM_ENTITY_PACKET = 0x8c,
    MCPE_TAKE_ITEM_ENTITY_PACKET = 0x8d,
    MCPE_MOVE_ENTITY_PACKET = 0x8e,
    MCPE_MOVE_PLAYER_PACKET = 0x8f,
    MCPE_REMOVE_BLOCK_PACKET = 0x90,
    MCPE_UPDATE_BLOCK_PACKET = 0x91,
    MCPE_ADD_PAINTING_PACKET = 0x92,
    MCPE_EXPLODE_PACKET = 0x93,
    MCPE_LEVEL_EVENT_PACKET = 0x94,
    MCPE_TILE_EVENT_PACKET = 0x95,
    MCPE_ENTITY_EVENT_PACKET = 0x96,
    MCPE_MOB_EFFECT_PACKET = 0x97,
    MCPE_PLAYER_EQUIPMENT_PACKET = 0x98,
    MCPE_PLAYER_ARMOR_EQUIPMENT_PACKET = 0x99,
    MCPE_INTERACT_PACKET = 0x9a,
    MCPE_USE_ITEM_PACKET = 0x9b,
    MCPE_PLAYER_ACTION_PACKET = 0x9c,
    MCPE_HURT_ARMOR_PACKET = 0x9d,
    MCPE_SET_ENTITY_DATA_PACKET = 0x9e,
    MCPE_SET_ENTITY_MOTION_PACKET = 0x9f,
    MCPE_SET_ENTITY_LINK_PACKET = 0xa0,
    MCPE_SET_HEALTH_PACKET = 0xa1,
    MCPE_SET_SPAWN_POSITION_PACKET = 0xa2,
    MCPE_ANIMATE_PACKET = 0xa3,
    MCPE_RESPAWN_PACKET = 0xa4,
    MCPE_DROP_ITEM_PACKET = 0xa5,
    MCPE_CONTAINER_OPEN_PACKET = 0xa6,
    MCPE_CONTAINER_CLOSE_PACKET = 0xa7,
    MCPE_CONTAINER_SET_SLOT_PACKET = 0xa8,
    MCPE_CONTAINER_SET_DATA_PACKET = 0xa9,
    MCPE_CONTAINER_SET_CONTENT_PACKET = 0xaa,
    MCPE_CONTAINER_ACK_PACKET = 0xab,
    MCPE_ADVENTURE_SETTINGS_PACKET = 0xac,
    MCPE_TILE_ENTITY_DATA_PACKET = 0xad,
    MCPE_PLAYER_INPUT_PACKET = 0xae,
    MCPE_FULL_CHUNK_DATA_PACKET = 0xaf,
    MCPE_SET_DIFFICULTY_PACKET = 0xb0,
    MCPE_BATCH_PACKET = 0xb1
};

class MCPEPlayer;

class MCPEPacket {
private:
    typedef MCPEPacket* CreatePacket();
    template<typename T> static MCPEPacket* packet() { return new T(); };

    static std::map<int, CreatePacket*> packets;

public:
    int id;
    MCPEChannel channel = MCPEChannel::NONE;

    template<typename T>
    static void registerPacket(int id) {
        MCPEPacket::packets[id] = MCPEPacket::packet<T>;
    }

    static void registerPackets();

    static MCPEPacket* getPacket(int id) {
        if (packets.count(id) > 0) {
            return packets.at(id)();
        }
        return null;
    };

    virtual void read(RakNet::BitStream& stream) { };
    virtual void write(RakNet::BitStream& stream) { };

    virtual void handle(MCPEPlayer& player) { };
};

class MCPELoginPacket : public MCPEPacket {
public:
    MCPELoginPacket() {
        id = MCPE_LOGIN_PACKET;
        channel = MCPEChannel::PRIORITY;
    };

    RakNet::RakString username;
    int protocol1, protocol2;
    int clientId;
    bool skinSlim;
    RakNet::RakString skin;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(username);
        stream.Read(protocol1);
        stream.Read(protocol2);
        stream.Read(clientId);
        unsigned char _skinSlim;
        stream.Read(_skinSlim);
        skinSlim = _skinSlim > 0;
        stream.Read(skin);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEPlayStatusPacket : public MCPEPacket {
public:
    MCPEPlayStatusPacket() {
        id = MCPE_PLAY_STATUS_PACKET;
        channel = MCPEChannel::PRIORITY;
    };

    enum class Status {
        SUCCESS, FAILED_CLIENT, FAILED_SERVER, PLAYER_SPAWN
    };

    Status status;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write((int) status);
    };
};

class MCPEDisconnectPacket : public MCPEPacket {
public:
    MCPEDisconnectPacket() {
        id = MCPE_DISCONNECT_PACKET;
        channel = MCPEChannel::PRIORITY;
    };

    RakNet::RakString message;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(message);
    };
};

class MCPEStartGamePacket : public MCPEPacket {
public:
    MCPEStartGamePacket() {
        id = MCPE_START_GAME_PACKET;
        channel = MCPEChannel::PRIORITY;
    };

    enum class Generator {
        OLD, INFINITE, FLAT
    };
    enum class GameMode {
        SURVIVAL, CREATIVE
    };

    int seed = 0;
    Generator generator = Generator::INFINITE;
    GameMode gamemode = GameMode::SURVIVAL;
    long long eid = 0;
    int spawnX, spawnY, spawnZ;
    float x, y, z;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(seed);
        stream.Write((int) generator);
        stream.Write((int) gamemode);
        stream.Write(eid);
        stream.Write(spawnX);
        stream.Write(spawnY);
        stream.Write(spawnZ);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
    };
};

class MCPERespawnPacket : public MCPEPacket {
public:
    MCPERespawnPacket() {
        id = MCPE_RESPAWN_PACKET;
        channel = MCPEChannel::WORLD_CHUNKS;
    };

    float x, y, z;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
    };
};

class Chunk;
class MCPEFullChunkDataPacket : public MCPEPacket {
public:
    MCPEFullChunkDataPacket() {
        id = MCPE_FULL_CHUNK_DATA_PACKET;
        channel = MCPEChannel::WORLD_CHUNKS;
    };

    Chunk* chunk;

    virtual void write(RakNet::BitStream& stream);
};