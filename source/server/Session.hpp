#pragma once

#include <arpa/inet.h>
#include <ctime>

#define MAX_SESSIONS_PER_USER 2

struct Session
{
    struct sockaddr_in address;
    std::time_t        lastAliveCheck;

    int                                                   sessionId;
    std::string                                           userHandle;
    int                                                   connectedSockets;
    std::array<struct sockaddr_in, MAX_SESSIONS_PER_USER> connectedPeers;
    std::vector<struct sockaddr_in>                       sockets;
};