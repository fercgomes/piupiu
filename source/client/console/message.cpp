#include "message.hpp"
#include <string>

struct ConsoleMessage MakeInfoMessage(std::string body, int timestamp)
{
    return ConsoleMessage(Notification, body, timestamp);
}

struct ConsoleMessage MakeErrorMessage(std::string body, int timestamp)
{
    return ConsoleMessage(Error, body, timestamp);
}

struct ConsoleMessage MakeNotificationMessage(std::string body, int timestamp)
{
    return ConsoleMessage(Info, body, timestamp);
}