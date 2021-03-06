#pragma once

#include <stdint.h>
#include <ctime>
#include <string.h>
#include <string>

#define PACKET_CONNECT_CMD 0
#define PACKET_DISCONNECT_CMD 1
#define PACKET_ACCEPT_CONN_CMD 2
#define PACKET_REJECT_CONN_CMD 3
#define PACKET_FOLLOW_CMD 4
#define PACKET_SEND_CMD 5
#define PACKET_NOTIFICATION 6
#define PACKET_ERROR 7
#define PACKET_INFO 8
#define PACKET_REQUEST_USER_INFO 9

#define PACKET_CONFIRM_STATE_CHANGE 10

#define PACKET_MAX_PAYLOAD_LEN 512

/*
    CONNECT
    DISCONNECT
    FOLLOW handle
    SEND message
*/

namespace Message
{
typedef struct
{
    uint16_t type;
    uint64_t seqn;
    uint16_t length; // payload length
    uint64_t timestamp;
    // Flag de destino: Primario | Secundario
    char payload[PACKET_MAX_PAYLOAD_LEN];
} Packet;

Packet MakeConnectCommand(uint64_t lastSeqn, std::string handle);
Packet MakeDisconnectCommand(uint64_t lastSeqn, std::string handle);
Packet MakeAcceptConnCommand(uint64_t lastSeqn);
Packet MakeRejectConnCommand(uint64_t lastSeqn);
Packet MakeFollowCommand(uint64_t lastSeqn, std::string handle);
Packet MakeSendCommand(uint64_t lastSeqn, std::string message);
Packet MakeNotification(uint64_t lastSeqn, std::string message, std::string sender);
Packet MakeError(uint64_t lastSeqn, std::string reason);
Packet MakeInfo(uint64_t lastSeqn, std::string message);
Packet MakeRequestUserInfo(uint64_t lastSeqn);

Packet MakeConfirmStateChangeMessage(uint64_t seqnToBeConfirmed);

const char* TypeToStr(uint16_t);

}; // namespace Message