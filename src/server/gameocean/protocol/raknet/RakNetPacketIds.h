#pragma once

static const unsigned char RakNetPacketMagic[16] = {0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe,
                                                    0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78};

enum RakNetPacketIds : unsigned char {

    RAKNET_PACKET_UNCONNECTED_PING = 1,
    RAKNET_PACKET_UNCONNECTED_PONG = 0x1C

};