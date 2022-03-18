#pragma once

#include <arpa/inet.h>
#include <ctime>
#include "Profile.hpp"

#define MAX_SESSIONS_PER_USER 2

struct Session
{
    struct sockaddr_in address;
    std::time_t        lastAliveCheck;

    int                             sessionId;
    std::string                     userHandle;
    Profile*                        profile;
    std::vector<struct sockaddr_in> sockets;
};