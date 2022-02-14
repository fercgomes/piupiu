#pragma once
#include <cstdint>
#include <arpa/inet.h>
#include <inttypes.h>
#include <vector>
#include "ProfileManager.hpp"

struct NotificationTarget
{
    struct sockaddr_in address;
    int                sent = 0;
};

struct Notification
{
    uint64_t                        id;
    uint32_t                        timestamp;
    Profile*                        author;
    std::string                     message;
    int                             sentCount;
    std::vector<NotificationTarget> targets;
};