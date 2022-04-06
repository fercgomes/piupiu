#pragma once
#include <ctime>
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
    std::time_t      timestamp;

    ConsoleMessage(enum MessageType type, std::string body) : type(type), body(body){};
    ConsoleMessage(enum MessageType type, std::string body, std::time_t timestamp)
        : type(type), body(body), timestamp(timestamp){};
};

struct ConsoleMessage MakeInfoMessage(std::string body, std::time_t timestamp);
struct ConsoleMessage MakeErrorMessage(std::string body, std::time_t timestamp);
struct ConsoleMessage MakeNotificationMessage(std::string body, std::time_t timestamp);