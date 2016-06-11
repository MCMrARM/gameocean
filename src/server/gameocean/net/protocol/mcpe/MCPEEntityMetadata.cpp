#include "MCPEEntityMetadata.h"

#include <gameocean/utils/BinaryStream.h>

void MCPEEntityMetadata::write(BinaryStream& stream) {
    for (auto const& pair : metadata) {
        int key = ((int) pair.second.type << 5) | pair.first;
        stream << (char) key;
        switch (pair.second.type) {
            case Type::BYTE:
                stream << (char) pair.second.intVal1;
                break;
            case Type::SHORT:
                stream << (short) pair.second.intVal1;
                break;
            case Type::INT:
                stream << pair.second.intVal1;
                break;
            case Type::FLOAT:
                stream << pair.second.floatVal1;
                break;
            case Type::STRING:
                stream << (short) pair.second.stringVal.length();
                stream.write((const byte*) pair.second.stringVal.c_str(), (int) pair.second.stringVal.length());
                break;
            case Type::ITEM_INSTANCE: {
                ItemInstance const& i = pair.second.itemVal;
                stream << (short) i.getItemId();
                if (i.getItemId() == 0)
                    return;
                stream << (char) i.count;
                stream << i.getItemData();
                stream << (short) 0;
            }
            case Type::INT_VECTOR3:
                stream << pair.second.intVal1;
                stream << pair.second.intVal2;
                stream << pair.second.intVal3;
                break;
            case Type::FLOAT_VECTOR3:
                stream << pair.second.floatVal1;
                stream << pair.second.floatVal2;
                stream << pair.second.floatVal3;
                break;
            case Type::LONG:
                stream << pair.second.longVal;
                break;
        }
    }
    stream << (char) 0x7f;
}