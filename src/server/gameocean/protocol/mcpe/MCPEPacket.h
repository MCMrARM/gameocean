#pragma once

#include <map>
#include <vector>
#include <memory>
#include <RakNet/BitStream.h>
#include <gameocean/common.h>
#include <gameocean/utils/UUID.h>
#include <gameocean/game/item/ItemInstance.h>

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
    void writeUUID(RakNet::BitStream& stream, UUID& uuid) {
        stream.Write(uuid.part1);
        stream.Write(uuid.part2);
    }
    ItemInstance readItemInstance(RakNet::BitStream& stream) {
        short id;
        stream.Read(id);
        if (id == 0)
            return ItemInstance ();

        byte count;
        short damage;
        stream.Read(count);
        stream.Read(damage);

        short nbtLen;
        stream.Read(nbtLen);
        if (nbtLen > 0) {
            byte n [nbtLen];
            stream.Read((char*) n, nbtLen);
        }

        return ItemInstance (id, count, damage);
    }
    void writeItemInstance(RakNet::BitStream& stream, ItemInstance& i) {
        stream.Write((short) i.getItemId());
        if (i.getItemId() == 0)
            return;
        stream.Write(i.count);
        stream.Write(i.getItemData());
        stream.Write((short) 0); // no nbt data
    }

public:
    int id;
    bool reliable = true;
    bool needsACK = false;
    bool priority = false;

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
    RakNet::RakString skinModel;
    std::string skin;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(username);
        stream.Read(protocol1);
        stream.Read(protocol2);
        stream.Read(clientId);
        clientUUID = readUUID(stream);
        stream.Read(serverAddress);
        stream.Read(clientSecret);
        stream.Read(skinModel);
        short skinLen;
        stream.Read(skinLen);
        if (skinLen <= 0)
            return;
        skin.resize(skinLen);
        stream.Read(&skin[0], skinLen);
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
        priority = true;
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

class MCPESetTimePacket : public MCPEPacket {
public:
    MCPESetTimePacket() {
        id = MCPE_SET_TIME_PACKET;
    };

    int time;
    bool started;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(time);
        writeBool(stream, started);
    };
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

class MCPEAddPlayerPacket : public MCPEPacket {
public:
    MCPEAddPlayerPacket() {
        id = MCPE_ADD_PLAYER_PACKET;
    };

    UUID uuid;
    RakNet::RakString username;
    long long eid = 0;
    float x, y, z;
    float speedX = 0.f;
    float speedY = 0.f;
    float speedZ = 0.f;
    float yaw, headYaw, pitch;
    ItemInstance item;

    virtual void write(RakNet::BitStream& stream) {
        writeUUID(stream, uuid);
        stream.Write(username);
        stream.Write(eid);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write(speedX);
        stream.Write(speedY);
        stream.Write(speedZ);
        stream.Write(yaw);
        stream.Write(headYaw);
        stream.Write(pitch);
        writeItemInstance(stream, item);

        stream.Write((byte) 127); // no meta
    };
};

class MCPERemovePlayerPacket : public MCPEPacket {
public:
    MCPERemovePlayerPacket() {
        id = MCPE_REMOVE_PLAYER_PACKET;
    };

    long long eid = 0;
    UUID uuid;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        writeUUID(stream, uuid);
    };
};

class MCPEMoveEntityPacket : public MCPEPacket {
public:
    MCPEMoveEntityPacket() {
        id = MCPE_MOVE_ENTITY_PACKET;
    };

    struct MoveEntry {
        long long eid;
        float x, y, z;
        float yaw, headYaw, pitch;
    };

    std::vector<MoveEntry> entries;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write((int) entries.size());
        for (MoveEntry e : entries) {
            stream.Write(e.eid);
            stream.Write(e.x);
            stream.Write(e.y);
            stream.Write(e.z);
            stream.Write(e.yaw);
            stream.Write(e.headYaw);
            stream.Write(e.pitch);
        }
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
    float yaw, headYaw, pitch;
    Mode mode;
    bool onGround;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write(yaw);
        stream.Write(headYaw);
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
        stream.Read(headYaw);
        stream.Read(pitch);
        stream.Read((byte&) mode);
        onGround = readBool(stream);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPERemoveBlockPacket : public MCPEPacket {
public:
    MCPERemoveBlockPacket() {
        id = MCPE_REMOVE_BLOCK_PACKET;
    };

    long long eid;
    int x, z;
    byte y;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(eid);
        stream.Read(x);
        stream.Read(z);
        stream.Read(y);
    };

    virtual void handle(MCPEPlayer& player);
};

class World;
class MCPEUpdateBlockPacket : public MCPEPacket {
public:
    MCPEUpdateBlockPacket() {
        id = MCPE_UPDATE_BLOCK_PACKET;
    }

    static const int FLAG_NEIGHBORS = 1;
    static const int FLAG_NETWORK = 2;
    static const int FLAG_NOGRAPHIC = 4;
    static const int FLAG_PRIORITY = 8;
    static const int FLAG_ALL = (FLAG_NEIGHBORS | FLAG_NETWORK);

    struct Entry {
        int x, z;
        byte y;
        byte blockId, blockMeta;
        byte flags;
    };
    std::vector<Entry> entries;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write((int) entries.size());
        for (Entry const& entry : entries) {
            stream.Write(entry.x);
            stream.Write(entry.z);
            stream.Write(entry.y);
            stream.Write(entry.blockId);
            stream.Write((byte) ((entry.flags << 4) | (int) entry.blockMeta));
        }
    };

    void add(World& world, int x, int y, int z, byte flags);
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

class MCPEUpdateAttributesPacket : public MCPEPacket {
public:
    MCPEUpdateAttributesPacket() {
        id = MCPE_UPDATE_ATTRIBUTES_PACKET;
    };

    static const char* ATTRIBUTE_HEALTH;
    static const char* ATTRIBUTE_HUNGER;
    static const char* ATTRIBUTE_EXPERIENCE;
    static const char* ATTRIBUTE_EXPERIENCE_LEVEL;

    struct Entry {
        float minValue, maxValue;
        float value;
        RakNet::RakString attribute;
    };

    long long eid;
    std::vector<Entry> entries;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        stream.Write((short) entries.size());
        for (Entry const& e : entries) {
            stream.Write(e.minValue);
            stream.Write(e.maxValue);
            stream.Write(e.value);
            stream.Write(e.attribute);
        }
    };
};

class MCPEMobEquipmentPacket : public MCPEPacket {
public:
    MCPEMobEquipmentPacket() {
        id = MCPE_MOB_EQUIPMENT_PACKET;
    };

    long long eid;
    ItemInstance item;
    byte slot, hotbarSlot;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        writeItemInstance(stream, item);
        stream.Write(slot);
        stream.Write(hotbarSlot);
    };

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(eid);
        item = readItemInstance(stream);
        stream.Read(slot);
        stream.Read(hotbarSlot);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEInteractPacket : public MCPEPacket {
public:
    MCPEInteractPacket() {
        id = MCPE_INTERACT_PACKET;
    };

    byte actionId;
    long long target;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(actionId);
        stream.Read(target);
    };

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(actionId);
        stream.Write(target);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEUseItemPacket : public MCPEPacket {
public:
    MCPEUseItemPacket() {
        id = MCPE_USE_ITEM_PACKET;
    };

    int x, y, z;
    byte side;
    float fx, fy, fz;
    float posX, posY, posZ;
    ItemInstance item;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(x);
        stream.Read(y);
        stream.Read(z);
        stream.Read(side);
        stream.Read(fx);
        stream.Read(fy);
        stream.Read(fz);
        stream.Read(posX);
        stream.Read(posY);
        stream.Read(posZ);
        item = readItemInstance(stream);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEPlayerActionPacket : public MCPEPacket {
public:
    MCPEPlayerActionPacket() {
        id = MCPE_PLAYER_ACTION_PACKET;
    };

    enum class Action {
        START_BREAK,
        ABORT_BREAK,
        STOP_BREAK,
        RELEASE_ITEM = 5,
        STOP_SLEEPING,
        RESPAWN,
        JUMP,
        START_SPRINT,
        STOP_SPRINT,
        START_SNEAK,
        STOP_SNEAK,
        DIMENSION_CHANGE
    };

    long long eid;
    Action action;
    int x, y, z;
    int side;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(eid);
        stream.Read((int&) action);
        stream.Read(x);
        stream.Read(y);
        stream.Read(z);
        stream.Read(side);
    };

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(eid);
        stream.Write((int) action);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
        stream.Write(side);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPESetHealthPacket : public MCPEPacket {
public:
    MCPESetHealthPacket() {
        id = MCPE_SET_HEALTH_PACKET;
    };

    int health;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(health);
    };
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

class MCPEContainerOpenPacket : public MCPEPacket {
public:
    MCPEContainerOpenPacket() {
        id = MCPE_CONTAINER_OPEN_PACKET;
    };

    byte window, type;
    short slots;
    int x, y, z;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(window);
        stream.Write(type);
        stream.Write(slots);
        stream.Write(x);
        stream.Write(y);
        stream.Write(z);
    };
};

class MCPEContainerClosePacket : public MCPEPacket {
public:
    MCPEContainerClosePacket() {
        id = MCPE_CONTAINER_CLOSE_PACKET;
    };

    byte window;

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(window);
    };

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(window);
    };
};

class MCPEContainerSetSlotPacket : public MCPEPacket {
public:
    MCPEContainerSetSlotPacket() {
        id = MCPE_CONTAINER_SET_SLOT_PACKET;
    };

    byte window;
    short slot, hotbar;
    ItemInstance item;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(window);
        stream.Write(slot);
        stream.Write(hotbar);
        writeItemInstance(stream, item);
    };

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(window);
        stream.Read(slot);
        stream.Read(hotbar);
        item = readItemInstance(stream);
    };
};

class MCPEContainerSetContentPacket : public MCPEPacket {
public:
    MCPEContainerSetContentPacket() {
        id = MCPE_CONTAINER_SET_CONTENT_PACKET;
    };

    byte window;
    std::vector<ItemInstance> items;
    std::vector<int> hotbar;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write(window);
        stream.Write((short) items.size());
        for (ItemInstance& i : items) {
            writeItemInstance(stream, i);
        }
        stream.Write((short) hotbar.size());
        for (int i : hotbar) {
            stream.Write(i);
        }
    };

    virtual void read(RakNet::BitStream& stream) {
        stream.Read(window);
        unsigned short itemCount;
        stream.Read(itemCount);
        items.resize(itemCount);
        for (ItemInstance& i : items) {
            i = readItemInstance(stream);
        }
        unsigned short hotbarCount;
        stream.Read(hotbarCount);
        hotbar.resize(hotbarCount);
        for (int& i : hotbar) {
            stream.Read(i);
        }
    };
};

class Tile;
class BinaryStream;
class MCPETileEntityDataPacket : public MCPEPacket {
public:
    MCPETileEntityDataPacket() {
        id = MCPE_TILE_ENTITY_DATA_PACKET;
    };

    std::shared_ptr<Tile> tile;

    virtual void write(RakNet::BitStream& stream);

    static void writeTile(BinaryStream& stream, Tile& tile);
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


class MCPEPlayerListPacket : public MCPEPacket {
public:
    MCPEPlayerListPacket() {
        id = MCPE_PLAYER_LIST_PACKET;
    };

    enum class Type : unsigned char {
        ADD, REMOVE
    };
    struct AddEntry {
        UUID uuid;
        long long eid;
        RakNet::RakString name;
        RakNet::RakString skinModel;
        std::string skin;
    };
    struct RemoveEntry {
        UUID uuid;
    };
    Type type;
    std::vector<AddEntry> addEntries;
    std::vector<RemoveEntry> removeEntries;

    virtual void write(RakNet::BitStream& stream) {
        stream.Write((unsigned char) type);
        if (type == Type::ADD) {
            stream.Write((int) addEntries.size());
            for (AddEntry e : addEntries) {
                writeUUID(stream, e.uuid);
                stream.Write(e.eid);
                stream.Write(e.name);
                stream.Write(e.skinModel);
                stream.Write((short) e.skin.size());
                stream.Write(e.skin.c_str(), (int) e.skin.size());
            }
        } else {
            stream.Write((int) removeEntries.size());
            for (RemoveEntry e : removeEntries) {
                writeUUID(stream, e.uuid);
            }
        }
    };
};