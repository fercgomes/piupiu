#pragma once

#include <string>
#include "../messaging/Packet.hpp"

typedef struct SocketAddress
{
    std::string  address;
    unsigned int port;

    SocketAddress() = default;
    SocketAddress(std::string address, int port) : address(address), port(port) {}
};

class Socket
{
private:
    SocketAddress boundAddress;
    int           socketDesc;

public:
    Socket();

    int Bind(std::string address, int port);
    int Send(SocketAddress address, Message::Packet data);
    int Receive(void* buffer, std::size_t bufferSize, SocketAddress& addr);

    bool Ready() const { return socketDesc != -1; }
};