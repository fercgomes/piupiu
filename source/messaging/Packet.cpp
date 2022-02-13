#include "Packet.hpp"
#include <iostream>

namespace Message
{

Packet MakeConnectCommand(uint64_t lastSeqn, std::string handle)
{
    Packet p = {.type      = PACKET_CONNECT_CMD,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    memset(p.payload, 0, PACKET_MAX_PAYLOAD_LEN);
    strcpy(p.payload, handle.c_str());

    return p;
}

Packet MakeDisconnectCommand(uint64_t lastSeqn, std::string handle)
{
    Packet p = {.type      = PACKET_DISCONNECT_CMD,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    memset(p.payload, 0, PACKET_MAX_PAYLOAD_LEN);
    strcpy(p.payload, handle.c_str());

    return p;
}

Packet MakeAcceptConnCommand(uint64_t lastSeqn)
{
    Packet p = {.type      = PACKET_ACCEPT_CONN_CMD,
                .seqn      = lastSeqn + 1,
                .length    = 0,
                .timestamp = std::time(nullptr)};

    return p;
}

Packet MakeRejectConnCommand(uint64_t lastSeqn)
{
    Packet p = {.type      = PACKET_REJECT_CONN_CMD,
                .seqn      = lastSeqn + 1,
                .length    = 0,
                .timestamp = std::time(nullptr)};

    return p;
}

Packet MakeFollowCommand(uint64_t lastSeqn, std::string handle)
{
    Packet p = {.type      = PACKET_FOLLOW_CMD,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    strcpy(p.payload, handle.c_str());

    return p;
}

Packet MakeSendCommand(uint64_t lastSeqn, std::string message)
{
    Packet p = {.type      = PACKET_REJECT_CONN_CMD,
                .seqn      = lastSeqn + 1,
                .length    = message.length(),
                .timestamp = std::time(nullptr)};

    strcpy(p.payload, message.c_str());

    return p;
}

Packet MakeNotification(uint64_t lastSeqn, std::string message)
{
    Packet p = {.type      = PACKET_REJECT_CONN_CMD,
                .seqn      = lastSeqn + 1,
                .length    = message.length(),
                .timestamp = std::time(nullptr)};

    strcpy(p.payload, message.c_str());

    return p;
}

const char* TypeToStr(uint16_t type)
{
    switch (type)
    {
    case PACKET_CONNECT_CMD:
    {
        return "PACKED_CONNECT_CMD";
    }
    case PACKET_DISCONNECT_CMD:
    {
        return "PACKET_DISCONNECT_CMD";
    }
    case PACKET_ACCEPT_CONN_CMD:
    {
        return "PACKET_ACCEPT_CONN_CMD";
    }
    case PACKET_REJECT_CONN_CMD:
    {
        return "PACKET_REJECT_CONN";
    }
    case PACKET_FOLLOW_CMD:
    {
        return "PACKET_FOLLOW_CMD";
    }
    case PACKET_SEND_CMD:
    {
        return "PACKET_SEND_CMD";
    }
    case PACKET_NOTIFICATION:
        return "PACKET_NOTIFICATION";
    }
}

}; // namespace Message