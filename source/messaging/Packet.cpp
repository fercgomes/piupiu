#include "Packet.hpp"
#include <iostream>
#include <sstream>

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

Packet MakeSendCommand(uint64_t lastSeqn, std::string handle)
{
    Packet p = {.type      = PACKET_SEND_CMD,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    strcpy(p.payload, handle.c_str());

    return p;
}

Packet MakeNotification(uint64_t lastSeqn, std::string handle, std::string sender)
{
    Packet p = {.type      = PACKET_NOTIFICATION,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    std::string output = "@" + sender + ":\n" + handle;

    strcpy(p.payload, output.c_str());

    return p;
}

Packet MakeElection(uint64_t lastSeqn)
{
    Packet p = {.type      = PACKET_ELECTION,
                .seqn      = lastSeqn + 1,
                .length    = 0,
                .timestamp = std::time(nullptr)};


     return p;
}


Packet MakeError(uint64_t lastSeqn, std::string reason)
{
    Packet p = {.type      = PACKET_ERROR,
                .seqn      = lastSeqn + 1,
                .length    = reason.length(),
                .timestamp = std::time(nullptr)};

    std::string output = "Error: " + reason;

    strcpy(p.payload, output.c_str());

    return p;
}

Packet MakeInfo(uint64_t lastSeqn, std::string handle)
{
    Packet p = {.type      = PACKET_INFO,
                .seqn      = lastSeqn + 1,
                .length    = handle.length(),
                .timestamp = std::time(nullptr)};

    std::string output = handle;

    strcpy(p.payload, output.c_str());

    return p;
}

Packet MakeRequestUserInfo(uint64_t lastSeqn)
{
    Packet p = {.type      = PACKET_REQUEST_USER_INFO,
                .seqn      = lastSeqn + 1,
                .length    = 0,
                .timestamp = std::time(nullptr)};

    return p;
}

Packet MakeConfirmStateChangeMessage(uint64_t seqnToBeConfirmed)
{
    Packet p = {.type      = PACKET_CONFIRM_STATE_CHANGE,
                .seqn      = seqnToBeConfirmed,
                .length    = 0,
                .timestamp = std::time(nullptr)};

    return p;
}

Packet MakeReply(uint64_t lastSeqn, std::string sender)
{
    Packet p = {.type      = PACKET_REPLY,
                .seqn      = lastSeqn,
                .length    = sender.length(),
                .timestamp = std::time(nullptr)};

    return p;
}

Packet Coordinator(uint64_t lastSeqn, std::string ip_addr, int port_number)
{
    Packet p = {.type      = PACKET_COORDINATOR,
                .seqn      = lastSeqn,
                .length    = 0,
                .timestamp = std::time(nullptr)};
    std::stringstream ss;
    ss << ip_addr << ":" << port_number;
    std::string output = ss.str();
    strcpy(p.payload, output.c_str());

    return p;
}

Packet MakeHeartbeatMessage()
{
    Packet p = {.type      = PACKET_HEARTBEAT,
                .seqn      = 0,
                .length    = 0,
                .timestamp = std::time(nullptr)};

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
    case PACKET_HEARTBEAT:
    {
        return "PACKET_HEARTBEAT";
    }
    case PACKET_ELECTION:
    {
        return "PACKET_ELECTION";
    }
    case PACKET_REPLY:
    {
        return "PACKET_REPLY";
    }
    case PACKET_COORDINATOR:
    {
        return "PACKET_COORDINATOR";
    }
    case PACKET_ERROR:
        return "PACKET_ERROR";
    case PACKET_NOTIFICATION:
        return "PACKET_NOTIFICATION";
    case PACKET_INFO:
        return "PACKET_INFO";
    case PACKET_CONFIRM_STATE_CHANGE:
        return "PACKET_CONFIRM_STATE_CHANGE";
    }
}

}; // namespace Message