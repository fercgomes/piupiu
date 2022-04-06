#pragma once
#include <string>

enum MessageType
{
    Notification,
    Info,
    Error
};

struct ConsoleMessage
{
    enum MessageType type;
    std::string      body;
    int              timestamp;

    ConsoleMessage(enum MessageType type, std::string body) : type(type), body(body){};
    ConsoleMessage(enum MessageType type, std::string body, int timestamp)
        : type(type), body(body), timestamp(timestamp){};
};

struct ConsoleMessage MakeInfoMessage(std::string body, int timestamp);
struct ConsoleMessage MakeErrorMessage(std::string body, int timestamp);
struct ConsoleMessage MakeNotificationMessage(std::string body, int timestamp);