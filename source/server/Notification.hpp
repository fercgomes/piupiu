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


//Idea - Using a bool value to check if a session was previous checked all the notifications
// If a user have two sessions opened into the same user, before the user open the notifications,
// this flag going to be put as FALSE, so we know the notfications hasn't been checked.
// If the flag was TRUE, one of the sessions was open the notifications and second one doesn't have new notifications
// If we receive new notifications, we put back the flag into FALSE so the sessions will know that we have new notifications
struct Notification
{
    uint64_t                        id;
    uint32_t                        timestamp;
    Profile*                        author;
    std::string                     message;
    int                             sentCount;
    std::vector<NotificationTarget> targets;
    bool                            notif_open;        
};