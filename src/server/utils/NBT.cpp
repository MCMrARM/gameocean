#include "NBT.h"

std::string NBTTag::getString(BinaryStream &stream) {
    unsigned short strLen;
    stream >> strLen;
    byte buf[strLen];
    stream.read(&buf[0], strLen);
    return std::string((char *) buf, strLen);
}

NBTTag* NBTTag::getTag(BinaryStream &stream, bool hasName, char dataType) {
    if (dataType == -1)
        stream >> dataType;

    if (dataType == 0)
        return new NBTEnd();

    std::string name;
    if (hasName)
        name = NBTTag::getString(stream);


    if (dataType == 1) {
        char val;
        stream >> val;
        return new NBTByte(name, val);
    } else if (dataType == 2) {
        short val;
        stream >> val;
        return new NBTShort(name, val);
    } else if (dataType == 3) {
        int val;
        stream >> val;
        return new NBTInt(name, val);
    } else if (dataType == 4) {
        long long val;
        stream >> val;
        return new NBTLong(name, val);
    } else if (dataType == 5) {
        float val;
        stream >> val;
        return new NBTFloat(name, val);
    } else if (dataType == 6) {
        double val;
        stream >> val;
        return new NBTDouble(name, val);
    } else if (dataType == 7) {
        unsigned int size;
        stream >> size;
        byte* buf = new byte[size];
        stream.read(&buf[0], size);
        return new NBTByteArray(name, buf, size);
    } else if (dataType == 8) {
        std::string val = NBTTag::getString(stream);
        return new NBTString(name, val);
    } else if (dataType == 9) {
        char listDataType;
        int count;
        stream >> listDataType >> count;
        std::vector<NBTTag*> val;
        for (int i = 0; i < count; i++) {
            NBTTag* tag = NBTTag::getTag(stream, false, listDataType);
            val.push_back(tag);
        }
        return new NBTList(name, val);
    } else if (dataType == 10) {
        std::map<std::string, NBTTag*> val;
        while (true) {
            NBTTag* tag = NBTTag::getTag(stream, true, -1);
            if (tag == null || tag->ID == 0) break;
            val[tag->name] = tag;
        }
        return new NBTCompound(name, val);
    } else if (dataType == 11) {
        int count;
        stream >> count;
        std::vector<int> val;
        int j;
        for (int i = 0; i < count; i++) {
            stream >> j;
            val.push_back(j);
        }
        return new NBTIntArray(name, val);
    }
    return null;
}