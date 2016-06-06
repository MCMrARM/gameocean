#pragma once

#include <map>
#include "Connection.h"
#include "../utils/BinaryStream.h"

class Socket;
class Connection;

#define PacketDef(id, type) \
    static const int ID = id; \
    static const Type TYPE = type; \
    virtual int getId() const { return ID; }; \
    virtual Type getType() { return TYPE; };

class Packet {

public:
    virtual ~Packet() {};

    virtual int getId() const = 0;

    enum class Type {
        CLIENTBOUND, SERVERBOUND, BOTH
    };

    virtual Type getType() = 0;

    virtual unsigned int getPacketSize() const = 0;
    virtual void read(BinaryStream &stream) = 0;
    virtual void write(BinaryStream &stream) = 0;

    virtual void handleClient(Connection &connection) {};
    virtual void handleServer(Connection &connection) {};

};

class UnknownPacket : public Packet {

public:
    int id;

    UnknownPacket(int id) : id(id) {};

    virtual Type getType() { return Type::BOTH; };

    virtual int getId() const { return id; };

    virtual unsigned int getPacketSize() const {
        return 0;
    };
    virtual void read(BinaryStream &stream) { };
    virtual void write(BinaryStream &stream) { };
};