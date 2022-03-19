#pragma once

#include <string>
#include "Session.hpp"

struct PendingNotification
{
    std::string senderUsername;
    std::string body;
    Session*    recipientSession;
    Profile*    recipient;
    int         sentCount;

    PendingNotification(std::string senderUsername, std::string body, Profile* recipient)
        : senderUsername(senderUsername), body(body), recipient(recipient)
    {
    }
};