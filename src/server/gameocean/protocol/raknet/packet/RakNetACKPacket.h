#pragma once

#include <gameocean/net/Packet.h>
#include "../RakNetPacketIds.h"
#include "../RakNetProtocol.h"

struct RakNetACKPacket : public Packet {
    PacketDef(RAKNET_PACKET_ACK, Type::BOTH)

    struct Range {
        unsigned int min, max;
    };

    std::vector<Range> ranges;

    RakNetACKPacket() {
    }

    void addPacketId(unsigned int i) {
        if (ranges.size() > 0) {
            if (ranges.back().max + 1 == i) {
                ranges.back().max++;
                return;
            }
        }
        ranges.push_back({i, i});
    }

    virtual unsigned int getPacketSize() const {
        unsigned int size = 0;
        for (auto it = ranges.begin(); it != ranges.end(); it++) {
            if (it->min == it->max) {
                size += 1 + 3;
            } else {
                size += 1 + 6;
            }
        }
        return BinaryStream::SHORT_SIZE + size;
    }

    virtual void read(BinaryStream& stream) {
        ranges.clear();
        unsigned short count;
        stream >> count;
        ranges.reserve(count);
        while (count--) {
            char mode;
            stream >> mode;
            if (mode == 0) {
                unsigned int min = 0, max = 0;
                stream.read((byte*) &min, 3);
                stream.read((byte*) &max, 3);
                ranges.push_back({min, max});
            } else if (mode == 1) {
                unsigned int val = 0;
                stream.read((byte*) &val, 3);
                ranges.push_back({val, val});
            }
        }
    }

    virtual void write(BinaryStream& stream) {
        stream << (unsigned short) ranges.size();
        for (auto it = ranges.begin(); it != ranges.end(); it++) {
            if (it->min == it->max) {
                stream << (char) 1;
                stream.write((byte*) &it->min, 3);
            } else {
                stream << (char) 0;
                stream.write((byte*) &it->min, 3);
                stream.write((byte*) &it->max, 3);
            }
        }
    }

    virtual void handleServer(Connection &connection);
};