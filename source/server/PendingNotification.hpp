#pragma once

#include <string>
#include "Session.hpp"

struct PendingNotification
{
    std::string senderUsername;
    std::string body;
    Session*    recipientSession;
    int         sentCount;

    PendingNotification(std::string senderUsername, std::string body, Session* recipientSession)
        : senderUsername(senderUsername), body(body), recipientSession(recipientSession)
    {
    }
};