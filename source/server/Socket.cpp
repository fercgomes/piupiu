#include "Socket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

Socket::Socket()
{
    // Init socket
    socketDesc = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketDesc != -1)
    {
        //
        std::cout << "[SOCKET] Socket inicializado (" << socketDesc << ")" << std::endl;
    }
    else
    {
        std::cerr << "[SOCKET] Falha ao inicializar o socket" << std::endl;
    }
}

int Socket::Bind(std::string address, int port)
{
    if (socketDesc != -1)
    {
        bindAddress = address;
        bindPort    = port;

        // Setar endereço do socket
        struct sockaddr_in addr;
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(port);
        addr.sin_addr.s_addr = inet_addr(address.c_str());

        // Fazer o bind do socket
        int r = bind(socketDesc, (struct sockaddr*)&addr, sizeof(addr));

        if (r >= 0) { std::cout << "[SOCKET] Socket bound (" << r << ")" << std::endl; }
        else
        {
            std::cerr << "[SOCKET] Socket bind failed" << std::endl;
        }
    }
    else
    {
        std::cerr << "[SOCKET] Socket não inicializado " << std::endl;
        return -1;
    }
}

int Socket::Send(std::string address, int port, Message::Packet data)
{
    if (socketDesc < 0)
    {
        std::cerr << "[SOCKET] Socket não inicializado " << std::endl;
        return -1;
    }

    struct sockaddr_in recipient;
    memset(&recipient, 0, sizeof(recipient));
    recipient.sin_family      = AF_INET;
    recipient.sin_port        = htons(port);
    recipient.sin_addr.s_addr = inet_addr(address.c_str());

    int r =
        sendto(socketDesc, &data, sizeof(data), 0, (struct sockaddr*)&recipient, sizeof(recipient));

    if (r < 0)
    {
        std::cerr << "[SOCKET] sendto() failed: " << r << std::endl;
        return r;
    }

    std::cout << "[SOCKET] sendto() sent " << r << " bytes" << std::endl;
    return r;
}