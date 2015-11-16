#pragma once

#include <map>
#include "Connection.h"
#include "../utils/BinaryStream.h"

class Socket;
class Connection;

class Packet {

private:
    typedef Packet* CreatePacket();
    template<typename T> static Packet* packet() { return new T(); };

    static std::map<int, CreatePacket*> clientPackets;
    static std::map<int, CreatePacket*> serverPackets;

public:
    virtual ~Packet() {};

    virtual int getId() const = 0;

    enum class Type {
        CLIENTBOUND, SERVERBOUND, BOTH
    };

    virtual Type getType() = 0;

    virtual unsigned int getPacketSize() const = 0;
    virtual void read(BinaryStream& stream) = 0;
    virtual void write(BinaryStream& stream) const = 0;

    virtual void handleClient(Connection &connection) {};
    virtual void handleServer(Connection &connection) {};

    template<typename T> static void registerPacket(int id, Packet::Type type);
    static void registerCommonPackets();

    static Packet* getPacket(int id, bool client);
    static Packet* getPacket(BinaryStream &stream, bool client);
    static void sendPacket(BinaryStream &stream, const Packet &packet);
    static void sendPacket(Connection &connection, const Packet &packet);

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
    virtual void read(BinaryStream& stream) { };
    virtual void write(BinaryStream& stream) const { };
};