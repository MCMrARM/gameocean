#include "NBT.h"

std::string NBTTag::getString(BinaryStream &stream) {
    unsigned short strLen;
    stream >> strLen;
    byte buf[strLen];
    stream.read(&buf[0], strLen);
    return std::string((char *) buf, strLen);
}

std::unique_ptr<NBTTag> NBTTag::getTag(BinaryStream &stream, bool hasName, char dataType) {
    if (dataType == -1)
        stream >> dataType;

    if (dataType == 0)
        return std::unique_ptr<NBTTag>(new NBTEnd());

    std::string name;
    if (hasName)
        name = NBTTag::getString(stream);


    if (dataType == 1) {
        char val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTByte(name, val));
    } else if (dataType == 2) {
        short val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTShort(name, val));
    } else if (dataType == 3) {
        int val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTInt(name, val));
    } else if (dataType == 4) {
        long long val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTLong(name, val));
    } else if (dataType == 5) {
        float val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTFloat(name, val));
    } else if (dataType == 6) {
        double val;
        stream >> val;
        return std::unique_ptr<NBTTag>(new NBTDouble(name, val));
    } else if (dataType == 7) {
        unsigned int size;
        stream >> size;
        std::unique_ptr<NBTByteArray> tag (new NBTByteArray(name));
        tag->val.resize(size);
        stream.read(&tag->val[0], size);
        return std::move(tag);
    } else if (dataType == 8) {
        std::string val = NBTTag::getString(stream);
        return std::unique_ptr<NBTTag>(new NBTString(name, val));
    } else if (dataType == 9) {
        char listDataType;
        int count;
        stream >> listDataType >> count;
        std::unique_ptr<NBTList> tag (new NBTList(name));
        for (int i = 0; i < count; i++) {
            tag->val.push_back(NBTTag::getTag(stream, false, listDataType));
        }
        return std::move(tag);
    } else if (dataType == 10) {
        std::unique_ptr<NBTCompound> rtag (new NBTCompound(name));
        while (true) {
            std::unique_ptr<NBTTag> tag = NBTTag::getTag(stream, true, -1);
            if (tag == null || tag->ID == 0) break;
            rtag->val[tag->name] = std::move(tag);
        }
        return std::move(rtag);
    } else if (dataType == 11) {
        int count;
        stream >> count;
        std::vector<int> val;
        int j;
        for (int i = 0; i < count; i++) {
            stream >> j;
            val.push_back(j);
        }
        return std::unique_ptr<NBTTag>(new NBTIntArray(name, val));
    }
    return null;
}