#include "message.hpp"
#include <string>

struct ConsoleMessage MakeInfoMessage(std::string body, std::time_t timestamp)
{
    return ConsoleMessage(Info, body, timestamp);
}

struct ConsoleMessage MakeErrorMessage(std::string body, std::time_t timestamp)
{
    return ConsoleMessage(Error, body, timestamp);
}

struct ConsoleMessage MakeNotificationMessage(std::string body, std::time_t timestamp)
{
    return ConsoleMessage(Notification, body, timestamp);
}