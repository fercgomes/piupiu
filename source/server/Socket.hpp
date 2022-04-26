#pragma once

#include <string>
#include "../messaging/Packet.hpp"

class Socket
{
private:
    std::string bindAddress;
    int         bindPort;
    int         socketDesc;

public:
    Socket();

    int Bind(std::string address, int port);
    int Send(std::string address, int port, Message::Packet data);
    int Receive(const char* buffer, std::size_t bufferSize, std::string& incomingAddress,
                int& incomingPort);

    bool Ready() const { return socketDesc != -1; }
};