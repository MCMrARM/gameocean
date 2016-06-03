#pragma once

#include <map>
#include <vector>
#include <memory>
#include <gameocean/common.h>
#include <gameocean/utils/UUID.h>
#include <gameocean/item/ItemInstance.h>
#include <gameocean/Player.h>
#include <gameocean/utils/BinaryStream.h>
#include <gameocean/world/ChunkPtr.h>
#include "MCPEEntityMetadata.h"

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
    MCPE_TELEMETRY_EVENT_PACKET = 0xc4,
    MCPE_SPAWN_EXPERIENCE_ORB_PACKET = 0xc5,
    MCPE_CLIENTBOUND_MAP_ITEM_DATA_PACKET = 0xc6,
    MCPE_MAP_INFO_REQUEST_PACKET = 0xc7,
    MCPE_REQUEST_CHUNK_RADIUS_PACKET = 0xc8,
    MCPE_CHUNK_RADIUS_UPDATE_PACKET = 0xc9,
    MCPE_ITEM_FRAME_DROP_ITEM_PACKET = 0xca,
    MCPE_REPLACE_SELECTED_ITEM_PACKET = 0xcb
};

class MCPEPlayer;

class MCPEPacket {
private:
    typedef MCPEPacket* CreatePacket();
    template<typename T> static MCPEPacket* packet() { return new T(); };

    static CreatePacket *packets[256];

protected:
    bool readBool(BinaryStream& stream) {
        byte r;
        stream >> r;
        return r;
    }
    void writeBool(BinaryStream& stream, bool b) {
        byte v = (byte) (b ? 1 : 0);
        stream << v;
    }
    UUID readUUID(BinaryStream& stream) {
        long long p1, p2;
        stream >> p1 >> p2;
        return UUID { p1, p2 };
    }
    void writeUUID(BinaryStream& stream, UUID const& uuid) {
        stream << uuid.part1 << uuid.part2;
    }
    ItemInstance readItemInstance(BinaryStream& stream) {
        short id;
        stream >> id;
        if (id == 0)
            return ItemInstance ();

        byte count;
        short damage;
        stream >> count >> damage;

        short nbtLen;
        stream >> nbtLen;
        if (nbtLen > 0) {
            byte n [nbtLen];
            stream.read(n, (unsigned int) nbtLen);
        }

        return ItemInstance (id, count, damage);
    }
    void writeItemInstance(BinaryStream& stream, ItemInstance const& i) {
        stream << (short) i.getItemId();
        if (i.getItemId() == 0)
            return;
        stream << i.count << i.getItemData();
        stream << (short) 0; // no nbt data
    }
    std::string readString(BinaryStream& stream) {
        unsigned int len;
        stream >> len;
        std::string str;
        str.resize(len);
        stream.read((byte*) &str[0], len);
        return str;
    }
    void writeString(BinaryStream& stream, std::string const& str) {
        stream << (unsigned int) str.size();
        stream.write((byte*) &str[0], (unsigned int) str.size());
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
        if (packets[id] != nullptr) {
            return packets[id]();
        }
        return nullptr;
    };

    virtual ~MCPEPacket() {}

    virtual void read(BinaryStream &stream) { }
    virtual void write(BinaryStream &stream) { }

    virtual void handle(MCPEPlayer &player) { }
};

class MCPELoginPacket : public MCPEPacket {
public:
    MCPELoginPacket() {
        id = MCPE_LOGIN_PACKET;
    };

    std::string username;
    int protocol1, protocol2;
    long long clientId;
    UUID clientUUID;
    std::string serverAddress;
    std::string clientSecret;
    std::string skinModel;
    std::string skin;

    virtual void read(BinaryStream& stream) {
        username = readString(stream);
        stream >> protocol1 >> protocol2 >> clientId;
        clientUUID = readUUID(stream);
        serverAddress = readString(stream);
        clientSecret = readString(stream);
        skinModel = readString(stream);
        unsigned short skinLen;
        stream >> skinLen;
        if (skinLen <= 0)
            return;
        skin.resize(skinLen);
        stream.read((byte*) &skin[0], skinLen);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEPlayStatusPacket : public MCPEPacket {
public:
    MCPEPlayStatusPacket() {
        id = MCPE_PLAY_STATUS_PACKET;
        priority = true;
    };

    enum class Status {
        SUCCESS, FAILED_CLIENT, FAILED_SERVER, PLAYER_SPAWN
    };

    Status status;

    virtual void write(BinaryStream& stream) {
        stream << (int) status;
    };
};

class MCPEDisconnectPacket : public MCPEPacket {
public:
    MCPEDisconnectPacket() {
        id = MCPE_DISCONNECT_PACKET;
        priority = true;
    };

    std::string message;

    virtual void write(BinaryStream& stream) {
        writeString(stream, message);
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
    std::string source;
    std::string message;
    std::vector<std::string> parameters;

    virtual void read(BinaryStream& stream) {
        stream >> (byte&) type;

        if (type == MessageType::POPUP || type == MessageType::CHAT)
            source = readString(stream);
        message = readString(stream);
        if (type == MessageType::TRANSLATION) {
            byte count;
            stream >> count;
            parameters.clear();
            parameters.resize(count);

            for (int i = 0; i < count; i++) {
                parameters.push_back(readString(stream));
            }
        }
    };

    virtual void write(BinaryStream& stream) {
        stream << (byte) type;

        if (type == MessageType::POPUP || type == MessageType::CHAT)
            writeString(stream, source);
        writeString(stream, message);
        if (type == MessageType::TRANSLATION) {
            byte count = (byte) parameters.size();
            stream << count;
            for (std::string p : parameters) {
                writeString(stream, p);
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

    virtual void write(BinaryStream& stream) {
        stream << time;
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
    bool isLoadedInCreative = true;
    byte dayCycleStopTime = 0;
    bool isEduMode = false;
    std::string levelId;

    virtual void write(BinaryStream& stream) {
        stream << seed;
        stream << (byte) dimension;
        stream << (int) generator;
        stream << (int) gamemode;
        stream << eid;
        stream << spawnX;
        stream << spawnY;
        stream << spawnZ;
        stream << x;
        stream << y;
        stream << z;
        writeBool(stream, isLoadedInCreative);
        stream << dayCycleStopTime;
        writeBool(stream, isEduMode);
        writeString(stream, levelId);
    };
};

class MCPEAddPlayerPacket : public MCPEPacket {
public:
    MCPEAddPlayerPacket() {
        id = MCPE_ADD_PLAYER_PACKET;
    };

    UUID uuid;
    std::string username;
    long long eid = 0;
    float x, y, z;
    float speedX = 0.f;
    float speedY = 0.f;
    float speedZ = 0.f;
    float yaw, headYaw, pitch;
    ItemInstance item;

    virtual void write(BinaryStream& stream) {
        writeUUID(stream, uuid);
        username = readString(stream);
        stream << eid;
        stream << x;
        stream << y;
        stream << z;
        stream << speedX;
        stream << speedY;
        stream << speedZ;
        stream << yaw;
        stream << headYaw;
        stream << pitch;
        writeItemInstance(stream, item);

        stream << (byte) 127; // no meta
    };
};

class MCPERemovePlayerPacket : public MCPEPacket {
public:
    MCPERemovePlayerPacket() {
        id = MCPE_REMOVE_PLAYER_PACKET;
    };

    long long eid = 0;
    UUID uuid;

    virtual void write(BinaryStream& stream) {
        stream << eid;
        writeUUID(stream, uuid);
    };
};

class MCPERemoveEntityPacket : public MCPEPacket {
public:
    MCPERemoveEntityPacket() {
        id = MCPE_REMOVE_ENTITY_PACKET;
    };

    long long eid = 0;

    virtual void write(BinaryStream& stream) {
        stream << eid;
    };
};

class MCPEAddEntityPacket : public MCPEPacket {
public:
    MCPEAddEntityPacket() {
        id = MCPE_ADD_ENTITY_PACKET;
    };

    long long eid;
    int typeId;
    float x, y, z;
    float motionX, motionY, motionZ;
    float yaw, pitch;
    MCPEEntityMetadata metadata;

    virtual void write(BinaryStream& stream) {
        stream << eid;
        stream << typeId;
        stream << x;
        stream << y;
        stream << z;
        stream << motionX;
        stream << motionY;
        stream << motionZ;
        stream << yaw;
        stream << pitch;
        metadata.write(stream);
        stream << (short) 0; // links
    };
};

class MCPEAddItemEntityPacket : public MCPEPacket {
public:
    MCPEAddItemEntityPacket() {
        id = MCPE_ADD_ITEM_ENTITY_PACKET;
    };

    long long eid;
    ItemInstance item;
    float x, y, z;
    float motionX, motionY, motionZ;

    virtual void write(BinaryStream& stream) {
        stream << eid;
        writeItemInstance(stream, item);
        stream << x;
        stream << y;
        stream << z;
        stream << motionX;
        stream << motionY;
        stream << motionZ;
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

    virtual void write(BinaryStream& stream) {
        stream << (unsigned int) entries.size();
        for (MoveEntry e : entries) {
            stream << e.eid;
            stream << e.x;
            stream << e.y;
            stream << e.z;
            stream << e.yaw;
            stream << e.headYaw;
            stream << e.pitch;
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

    virtual void write(BinaryStream& stream) {
        stream << eid;
        stream << x;
        stream << y;
        stream << z;
        stream << yaw;
        stream << headYaw;
        stream << pitch;
        stream << (byte) mode;
        writeBool(stream, onGround);
    };

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        stream >> x;
        stream >> y;
        stream >> z;
        stream >> yaw;
        stream >> headYaw;
        stream >> pitch;
        stream >> (byte&) mode;
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

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        stream >> x;
        stream >> z;
        stream >> y;
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

    virtual void write(BinaryStream& stream) {
        stream << (int) entries.size();
        for (Entry const& entry : entries) {
            stream << entry.x;
            stream << entry.z;
            stream << entry.y;
            stream << entry.blockId;
            stream << (byte) ((entry.flags << 4) | (int) entry.blockMeta);
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

    virtual void write(BinaryStream& stream) {
        stream << eid;
        stream << (byte) event;
    };

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        stream >> (byte&) event;
    };

    virtual void handle(MCPEPlayer& player);
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
        std::string attribute;
    };

    long long eid;
    std::vector<Entry> entries;

    virtual void write(BinaryStream& stream) {
        stream << eid;
        stream << (short) entries.size();
        for (Entry const& e : entries) {
            stream << e.minValue;
            stream << e.maxValue;
            stream << e.value;
            stream << e.attribute;
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

    virtual void write(BinaryStream& stream) {
        stream << eid;
        writeItemInstance(stream, item);
        stream << slot;
        stream << hotbarSlot;
    };

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        item = readItemInstance(stream);
        stream >> slot;
        stream >> hotbarSlot;
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEMobArmorEquipmentPacket : public MCPEPacket {
public:
    MCPEMobArmorEquipmentPacket() {
        id = MCPE_MOB_ARMOR_EQUIPMENT_PACKET;
    };

    long long eid;
    ItemInstance slots[4];

    virtual void write(BinaryStream& stream) {
        stream << eid;
        for (int i = 0; i < 4; i++)
            writeItemInstance(stream, slots[i]);
    };

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        for (int i = 0; i < 4; i++)
            slots[i] = readItemInstance(stream);
    };
};

class MCPEInteractPacket : public MCPEPacket {
public:
    MCPEInteractPacket() {
        id = MCPE_INTERACT_PACKET;
    };

    byte actionId;
    long long target;

    virtual void read(BinaryStream& stream) {
        stream >> actionId;
        stream >> target;
    };

    virtual void write(BinaryStream& stream) {
        stream << actionId;
        stream << target;
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

    virtual void read(BinaryStream& stream) {
        stream >> x;
        stream >> y;
        stream >> z;
        stream >> side;
        stream >> fx;
        stream >> fy;
        stream >> fz;
        stream >> posX;
        stream >> posY;
        stream >> posZ;
        item = readItemInstance(stream);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPESetEntityMotionPacket : public MCPEPacket {
public:
    MCPESetEntityMotionPacket() {
        id = MCPE_SET_ENTITY_MOTION_PACKET;
    };

    struct Entry {
        long long eid;
        float x, y, z;
    };

    std::vector<Entry> entries;

    virtual void write(BinaryStream& stream) {
        stream << (unsigned int) entries.size();
        for (Entry e : entries) {
            stream << e.eid;
            stream << e.x << e.y << e.z;
        }
    };
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

    virtual void read(BinaryStream& stream) {
        stream >> eid;
        stream >> (int&) action;
        stream >> x >> y >> z;
        stream >> side;
    };

    virtual void write(BinaryStream& stream) {
        stream << eid;
        stream << (int) action;
        stream << x << y << z;
        stream << side;
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPESetHealthPacket : public MCPEPacket {
public:
    MCPESetHealthPacket() {
        id = MCPE_SET_HEALTH_PACKET;
    };

    int health;

    virtual void write(BinaryStream& stream) {
        stream << health;
    };
};

class MCPERespawnPacket : public MCPEPacket {
public:
    MCPERespawnPacket() {
        id = MCPE_RESPAWN_PACKET;
    };

    float x, y, z;

    virtual void write(BinaryStream& stream) {
        stream << x << y << z;
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

    virtual void write(BinaryStream& stream) {
        stream << window;
        stream << type;
        stream << slots;
        stream << x << y << z;
    };
};

class MCPEContainerClosePacket : public MCPEPacket {
public:
    MCPEContainerClosePacket() {
        id = MCPE_CONTAINER_CLOSE_PACKET;
    };

    byte window;

    virtual void read(BinaryStream& stream) {
        stream >> window;
    };

    virtual void write(BinaryStream& stream) {
        stream << window;
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEContainerSetSlotPacket : public MCPEPacket {
public:
    MCPEContainerSetSlotPacket() {
        id = MCPE_CONTAINER_SET_SLOT_PACKET;
    };

    byte window;
    short slot, hotbar;
    ItemInstance item;

    virtual void write(BinaryStream& stream) {
        stream << window;
        stream << slot;
        stream << hotbar;
        writeItemInstance(stream, item);
    };

    virtual void read(BinaryStream& stream) {
        stream >> window;
        stream >> slot;
        stream >> hotbar;
        item = readItemInstance(stream);
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEContainerSetContentPacket : public MCPEPacket {
public:
    MCPEContainerSetContentPacket() {
        id = MCPE_CONTAINER_SET_CONTENT_PACKET;
    };

    byte window;
    std::vector<ItemInstance> items;
    std::vector<int> hotbar;

    virtual void write(BinaryStream& stream) {
        stream << window;
        stream << (short) items.size();
        for (ItemInstance& i : items) {
            writeItemInstance(stream, i);
        }
        stream << (short) hotbar.size();
        for (int i : hotbar) {
            stream << i;
        }
    };

    virtual void read(BinaryStream& stream) {
        stream >> window;
        unsigned short itemCount;
        stream >> itemCount;
        items.resize(itemCount);
        for (ItemInstance& i : items) {
            i = readItemInstance(stream);
        }
        unsigned short hotbarCount;
        stream >> hotbarCount;
        hotbar.resize(hotbarCount);
        for (int& i : hotbar) {
            stream >> i;
        }
    };
};

class Recipe;

class MCPECraftingDataPacket : public MCPEPacket {
public:
    MCPECraftingDataPacket() {
        id = MCPE_CRAFTING_DATA_PACKET;
    };

    std::map<int, Recipe*> recipes;
    bool clearRecipes = true;

    virtual void write(BinaryStream& stream);
};

class MCPECraftingEventPacket : public MCPEPacket {
public:
    MCPECraftingEventPacket() {
        id = MCPE_CRAFTING_EVENT_PACKET;
    };

    byte window;
    int type;
    UUID uuid;
    std::vector<ItemInstance> input;
    std::vector<ItemInstance> output;

    virtual void read(BinaryStream& stream) {
        stream >> window;
        stream >> type;
        uuid = readUUID(stream);
        int count;
        stream >> count;
        for (int i = 0; i < count; i++) {
            input.push_back(readItemInstance(stream));
        }
        stream >> count;
        for (int i = 0; i < count; i++) {
            output.push_back(readItemInstance(stream));
        }
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEAdventureSettingsPacket : public MCPEPacket {
public:
    MCPEAdventureSettingsPacket() {
        id = MCPE_ADVENTURE_SETTINGS_PACKET;
        flags.asInt = 0;
    };

    struct Flags {
        bool worldInmutable : 1;
        bool noPvP : 1;
        bool noPvM : 1;
        bool noMvP : 1;
        bool staticTime : 1;
        bool nametagsVisible : 1;
        bool autoJump : 1;
        bool allowFly : 1;
        bool noclip : 1;
        int filler : 23;
    };
    union FlagsUnion {
        Flags flags;
        int asInt;
    };

    FlagsUnion flags;
    int userPermissions = 2;
    int globalPermissions = 2;

    virtual void write(BinaryStream& stream) {
        stream << flags.asInt;
        stream << userPermissions;
        stream << globalPermissions;
    }
};

class Tile;
class BinaryStream;
class MCPETileEntityDataPacket : public MCPEPacket {
public:
    MCPETileEntityDataPacket() {
        id = MCPE_TILE_ENTITY_DATA_PACKET;
    };

    std::shared_ptr<Tile> tile;

    virtual void write(BinaryStream& stream);

    static void writeTile(BinaryStream& stream, Tile& tile);
};

class Chunk;
class MCPEFullChunkDataPacket : public MCPEPacket {
public:
    MCPEFullChunkDataPacket() {
        id = MCPE_FULL_CHUNK_DATA_PACKET;
    };

    ChunkPtr chunk;

    virtual void write(BinaryStream& stream);
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
        std::string name;
        std::string skinModel;
        std::string skin;
    };
    struct RemoveEntry {
        UUID uuid;
    };
    Type type;
    std::vector<AddEntry> addEntries;
    std::vector<RemoveEntry> removeEntries;

    virtual void write(BinaryStream& stream) {
        stream << (unsigned char) type;
        if (type == Type::ADD) {
            stream << (unsigned int) addEntries.size();
            for (AddEntry e : addEntries) {
                writeUUID(stream, e.uuid);
                stream << e.eid;
                stream << e.name;
                stream << e.skinModel;
                stream << (short) e.skin.size();
                stream.write((byte*) e.skin.c_str(), (unsigned int) e.skin.size());
            }
        } else {
            stream << (unsigned int) removeEntries.size();
            for (RemoveEntry e : removeEntries) {
                writeUUID(stream, e.uuid);
            }
        }
    };
};

class MCPERequestChunkRadiusPacket : public MCPEPacket {
public:
    MCPERequestChunkRadiusPacket() {
        id = MCPE_REQUEST_CHUNK_RADIUS_PACKET;
    };

    int radius;

    virtual void read(BinaryStream& stream) {
        stream >> radius;
    };

    virtual void handle(MCPEPlayer& player);
};

class MCPEChunkRadiusUpdatePacket : public MCPEPacket {
public:
    MCPEChunkRadiusUpdatePacket() {
        id = MCPE_CHUNK_RADIUS_UPDATE_PACKET;
    };

    int radius;

    virtual void write(BinaryStream& stream) {
        stream << radius;
    };
};