#pragma once

static const unsigned char RakNetPacketMagic[16] = {0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe,
                                                    0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78};

enum RakNetPacketIds : unsigned char {

    RAKNET_PACKET_PING = 0,
    RAKNET_PACKET_UNCONNECTED_PING = 1,
    RAKNET_PACKET_PONG = 3,
    RAKNET_PACKET_CONNECT_MTU_REQUEST = 5,
    RAKNET_PACKET_CONNECT_MTU_REPLY = 6,
    RAKNET_PACKET_CONNECT_REQUEST = 7,
    RAKNET_PACKET_CONNECT_REPLY = 8,
    RAKNET_PACKET_ONLINE_CONNECT_REQUEST = 9,
    RAKNET_PACKET_ONLINE_CONNECT_REPLY = 0x10,
    RAKNET_PACKET_CLIENT_HANDSHAKE = 0x13,
    RAKNET_PACKET_UNCONNECTED_PONG = 0x1C,
    RAKNET_PACKET_NAK = 0xA0,
    RAKNET_PACKET_ACK = 0xC0

};