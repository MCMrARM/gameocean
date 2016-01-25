#pragma once

#include <string>
#include <map>
#include <gameocean/item/ItemInstance.h>

class BinaryStream;

struct MCPEEntityMetadata {

public:
    static const int FLAGS = 0;
    static const int AIR = 1;
    static const int NAMETAG = 2;
    static const int SHOW_NAMETAG = 3;
    static const int SILENT = 4;
    static const int POTION_COLOR = 7;
    static const int POTION_AMBIENT = 8;
    static const int NO_AI = 15;

    enum class Type {
        BYTE, SHORT, INT, FLOAT, STRING, ITEM_INSTANCE, INT_VECTOR3, FLOAT_VECTOR3, LONG
    };

    struct MetadataValue {
        Type type;
        int intVal1, intVal2, intVal3;
        float floatVal1, floatVal2, floatVal3;
        long long longVal;
        ItemInstance itemVal;
        std::string stringVal;

        MetadataValue() {
            //
        }
        MetadataValue(Type type, int val) : type(type), intVal1(val) {
            //
        }
        MetadataValue(int val1, int val2, int val3) : type(Type::INT_VECTOR3), intVal1(val1), intVal2(val2), intVal3(val3) {
            //
        }
        MetadataValue(float val) : type(Type::FLOAT), floatVal1(val) {
            //
        }
        MetadataValue(float val1, float val2, float val3) : type(Type::FLOAT_VECTOR3), floatVal1(val1), floatVal2(val2), floatVal3(val3) {
            //
        }
        MetadataValue(long long val) : type(Type::LONG), longVal(val) {
            //
        }
        MetadataValue(ItemInstance itemInstance) : type(Type::ITEM_INSTANCE), itemVal(itemVal) {
            //
        }
        MetadataValue(std::string str) : type(Type::STRING), stringVal(str) {
            //
        }
    };

    std::map<int, MetadataValue> metadata;

    inline void setByte(int id, char val) { metadata[id] = MetadataValue (Type::BYTE, (int) val); }
    inline void setShort(int id, short val) { metadata[id] = MetadataValue (Type::SHORT, (int) val); }
    inline void setInt(int id, int val) { metadata[id] = MetadataValue (Type::INT, val); }
    inline void setLong(int id, long long val) { metadata[id] = MetadataValue (val); }
    inline void setFloat(int id, float val) { metadata[id] = MetadataValue (val); }
    inline void setString(int id, std::string val) { metadata[id] = MetadataValue (val); }
    inline void setItemInstance(int id, std::string val) { metadata[id] = MetadataValue (val); }
    inline void setIntVec3(int id, int x, int y, int z) { metadata[id] = MetadataValue (x, y, z); }
    inline void setFloatVec3(int id, float x, float y, float z) { metadata[id] = MetadataValue (x, y, z); }

    void write(BinaryStream& stream);

};