#pragma once

#include <map>
#include <vector>
#include <RakNet/BitStream.h>
#include "common.h"
#include "utils/UUID.h"

enum MCPEMessages {
    MCPE_LOGIN_PACKET = 0x8f,
    MCPE_PLAY_STATUS_PACKET = 0x90,
    MCPE_DISCONNECT_PACKET = 0x91,
    MCPE_BATCH_PACKET = 0x92,
    MCPE_TEXT_PACKET = 0x93,
    MCPE_SET_TIME_PACKET = 0x94,
    MCPE_START_GAME_PACKET = 0x95,
    MCPE_ADD_PLAYER_PACKET = 0x96,
    MCPE_REMOVE_PLAYER_PACKET = 0x97,
    MCPE_ADD_ENTITY_PACKET = 0x98,
    MCPE_REMOVE_ENTITY_PACKET = 0x99,
    MCPE_ADD_ITEM_ENTITY_PACKET = 0x9a,
    MCPE_TAKE_ITEM_ENTITY_PACKET = 0x9b,
    MCPE_MOVE_ENTITY_PACKET = 0x9c,
    MCPE_MOVE_PLAYER_PACKET = 0x9d,
    MCPE_REMOVE_BLOCK_PACKET = 0x9e,
    MCPE_UPDATE_BLOCK_PACKET = 0x9f,
    MCPE_ADD_PAINTING_PACKET = 0xa0,
    MCPE_EXPLODE_PACKET = 0xa1,
    MCPE_LEVEL_EVENT_PACKET = 0xa2,
    MCPE_TILE_EVENT_PACKET = 0xa3,
    MCPE_ENTITY_EVENT_PACKET = 0xa4,
    MCPE_MOB_EFFECT_PACKET = 0xa5,
    MCPE_UPDATE_ATTRIBUTES_PACKET = 0xa6,
    MCPE_MOB_EQUIPMENT_PACKET = 0xa7,
    MCPE_MOB_ARMOR_EQUIPMENT_PACKET = 0xa8,
    MCPE_INTERACT_PACKET = 0xa9,
    MCPE_USE_ITEM_PACKET = 0xaa,
    MCPE_PLAYER_ACTION_PACKET = 0xab,
    MCPE_HURT_ARMOR_PACKET = 0xac,
    MCPE_SET_ENTITY_DATA_PACKET = 0xad,
    MCPE_SET_ENTITY_MOTION_PACKET = 0xae,
    MCPE_SET_ENTITY_LINK_PACKET = 0xaf,
    MCPE_SET_HEALTH_PACKET = 0xb0,
    MCPE_SET_SPAWN_POSITION_PACKET = 0xb1,
    MCPE_ANIMATE_PACKET = 0xb2,
    MCPE_RESPAWN_PACKET = 0xb3,
    MCPE_DROP_ITEM_PACKET = 0xb4,
    MCPE_CONTAINER_OPEN_PACKET = 0xb5,
    MCPE_CONTAINER_CLOSE_PACKET = 0xb6,
    MCPE_CONTAINER_SET_SLOT_PACKET = 0xb7,
    MCPE_CONTAINER_SET_DATA_PACKET = 0xb8,
    MCPE_CONTAINER_SET_CONTENT_PACKET = 0xb9,
    MCPE_CRAFTING_DATA_PACKET = 0xba,
    MCPE_CRAFTING_EVENT_PACKET = 0xbb,
    MCPE_ADVENTURE_SETTINGS_PACKET = 0xbc,
    MCPE_TILE_ENTITY_DATA_PACKET = 0xbd,
    MCPE_PLAYER_INPUT_PACKET = 0xbe,
    MCPE_FULL_CHUNK_DATA_PACKET = 0xbf,
    MCPE_SET_DIFFICULTY_PACKET = 0xc0,
    MCPE_CHANGE_DIMENSION_PACKET = 0xc1,
    MCPE_SET_PLAYER_GAMETYPE_PACKET = 0xc2,
    MCPE_PLAYER_LIST_PACKET = 0xc3,
    MCPE_TELEMETRY_EVENT_PACKET = 0xc4
};

class MCPEPlayer;

class MCPEPacket {
private:
    typedef MCPEPacket* CreatePacket();
    template<typename T> static MCPEPacket* packet() { return new T(); };

    static std::map<int, CreatePacket*> packets;

protected:
    bool readBool(RakNet::BitStream& stream) {
        byte r;
        stream.Read(r);
        return r;
    }
    void writeBool(RakNet::BitStream& stream, bool b) {
        byte v = b ? 1 : 0;
        stream.Write(v);
    }
    UUID readUUID(RakNet::BitStream& stream) {
        long long p1, p2;
        stream.Read(p1);
        stream.Read(p2);
        return UUID { p1, p2 };
    }
    void writeUUID(RakNet::BitStream& stream, UUID uuid) {
        stream.Write(uuid.part1);
        stream.Write(uuid.part2);
    }

public:
    int id;
    bool reliable = true;
    bool needsACK = false;

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
    };

    RakNet::RakString username;
    int protocol1, protocol2;
    long long clientId;
    UUID clientUUID;
    RakNet::RakString serverAddress;
    RakNet::RakString clientSecret;
    bool skinSlim;
    RakNet::RakString skin;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(username);
        stream.Read(protocol1);
        stream.Read(protocol2);
        stream.Read(clientId);
        clientUUID = readUUID(stream);
        stream.Read(serverAddress);
        stream.Read(clientSecret);
        skinSlim = readBool(stream);
        stream.Read(skin);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEPlayStatusPacket : public MCPEPacket {
public:
    MCPEPlayStatusPacket() {
        id = MCPE_PLAY_STATUS_PACKET;
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
    };

    RakNet::RakString message;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(message);
    };
};

class MCPETextPacket : public MCPEPacket {
public:
    MCPETextPacket() {
        id = MCPE_TEXT_PACKET;
    };

    enum class MessageType {
        RAW, CHAT, TRANSLATION, POPUP, TIP, SYSTEM
    };

    MessageType type = MessageType::RAW;
    RakNet::RakString source;
    RakNet::RakString message;
    std::vector<RakNet::RakString> parameters;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read((byte&) type);

        if (type == MessageType::POPUP || type == MessageType::CHAT)
            stream.Read(source);
        stream.Read(message);
        if (type == MessageType::TRANSLATION) {
            byte count;
            stream.Read(count);
            parameters.clear();
            parameters.resize(count);

            RakNet::RakString p;
            for (int i = 0; i < count; i++) {
                stream.Read(p);
                parameters.push_back(p);
            }
        }
    };

    virtual void write(RakNet::BitStream& stream) {
        stream.Write((byte) type);

        if (type == MessageType::POPUP || type == MessageType::CHAT)
            stream.Write(source);
        stream.Write(message);
        if (type == MessageType::TRANSLATION) {
            byte count = (byte) parameters.size();
            stream.Write(count);
            for (RakNet::RakString p : parameters) {
                stream.Write(p);
            }
        }
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEStartGamePacket : public MCPEPacket {
public:
    MCPEStartGamePacket() {
        id = MCPE_START_GAME_PACKET;
    };

    enum class Generator {
        OLD, INFINITE, FLAT
    };
    enum class Dimension {
        OVERWORLD, NETHER, END
    };
    enum class GameMode {
        SURVIVAL, CREATIVE
    };

    int seed = 0;
    Generator generator = Generator::INFINITE;
    Dimension dimension = Dimension::OVERWORLD;
    GameMode gamemode = GameMode::SURVIVAL;
    long long eid = 0;
    int spawnX, spawnY, spawnZ;
    float x, y, z;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(seed);
        stream.Write((byte) dimension);
        stream.Write((int) generator);
        stream.Write((int) gamemode);
        stream.Write(eid);
        stream.Write(spawnX);
        stream.Write(spawnY);
        stream.Write(spawnZ);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write((byte) 0); // todo: what is this?
    };
};

class MCPEMovePlayerPacket : public MCPEPacket {
public:
    MCPEMovePlayerPacket() {
        id = MCPE_MOVE_PLAYER_PACKET;
    };

    enum class Mode : byte {
        NORMAL, RESET, ROTATION
    };

    long long eid;
    float x, y, z;
    float yaw, bodyYaw, pitch;
    Mode mode;
    bool onGround;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write(yaw);
        stream.Write(bodyYaw);
        stream.Write(pitch);
        stream.Write((byte) mode);
        writeBool(stream, onGround);
    };

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(eid);
        stream.Read(x);
        stream.Read(y);
        stream.Read(z);
        stream.Read(yaw);
        stream.Read(bodyYaw);
        stream.Read(pitch);
        stream.Read((byte&) mode);
        onGround = readBool(stream);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEEntityEventPacket : public MCPEPacket {
public:
    MCPEEntityEventPacket() {
        id = MCPE_ENTITY_EVENT_PACKET;
    }

    enum class Event : byte {
        HURT_ANIMATION = 2,
        DEATH_ANIMATION = 3,
        TAME_FAIL = 6,
        TAME_SUCCESS = 7,
        SHAKE_WET = 8,
        USE_ITEM = 9,
        EAT_GRASS_ANIMATION = 10,
        FISH_HOOK_BUBBLE = 11,
        FISH_HOOK_POSITION = 12,
        FISH_HOOK_HOOK = 13,
        FISH_HOOK_TEASE = 14,
        SQUID_INK_CLOUD = 15,
        AMBIENT_SOUND = 16,
        RESPAWN = 17
    };

    long long eid;
    Event event;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        stream.Write((byte) event);
    };

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(eid);
        stream.Read((byte&) event);
    };
};

class MCPEUseItemPacket : public MCPEPacket {
public:
    MCPEUseItemPacket() {
        id = MCPE_USE_ITEM_PACKET;
    };

    int x, y, z;
    byte side;
    short itemId, itemDamage;
    long long eid;
    float fx, fy, fz;
    float posX, posY, posZ;

    virtual void read(RakNet::BitStream& stream) {
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write(side);
        stream.Write(itemId);
        stream.Write(itemDamage);
        stream.Write(eid);
        stream.Write(fx);
        stream.Write(fy);
        stream.Write(fz);
        stream.Write(posX);
        stream.Write(posY);
        stream.Write(posZ);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPERespawnPacket : public MCPEPacket {
public:
    MCPERespawnPacket() {
        id = MCPE_RESPAWN_PACKET;
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
    };

    Chunk* chunk;

    virtual void write(RakNet::BitStream& stream);
};