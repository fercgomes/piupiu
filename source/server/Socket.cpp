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
        boundAddress.address = address;
        boundAddress.port    = port;
        std::cout << "[SOCKET] Binding " << address << ":" << port << std::endl;

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

int Socket::Send(SocketAddress address, Message::Packet data)
{
    if (socketDesc < 0)
    {
        std::cerr << "[SOCKET] Socket não inicializado " << std::endl;
        return -1;
    }

    std::cout << "[SOCKET] Sending to host " << address.address << ":" << address.port << std::endl;

    struct sockaddr_in recipient;
    memset(&recipient, 0, sizeof(recipient));
    recipient.sin_family      = AF_INET;
    recipient.sin_port        = htons(address.port);
    recipient.sin_addr.s_addr = inet_addr(address.address.c_str());

    int r =
        sendto(socketDesc, &data, sizeof(data), 0, (struct sockaddr*)&recipient, sizeof(recipient));

    if (r < 0)
    {
        std::cerr << "[SOCKET] sendto() failed: " << r << std::endl;
        return r;
    }

    std::cout << "[SOCKET] sendto() sent " << r << " bytes (" << Message::TypeToStr(data.type)
              << ")" << std::endl;
    return r;
}
int Socket::Receive(void* buffer, std::size_t bufferSize, SocketAddress& addr)
{
    struct sockaddr_in incAddr;
    socklen_t          incAddrlen;

    memset(&incAddr, 0, sizeof(incAddr));
    // memset(&incAddrlen, 0, sizeof(socklen_t));
    incAddrlen = sizeof(struct sockaddr_in);

    int r = recvfrom(socketDesc, buffer, bufferSize, 0, (struct sockaddr*)&incAddr, &incAddrlen);

    if (r > 0)
    {

        addr.address = std::string(inet_ntoa(incAddr.sin_addr));
        addr.port    = incAddr.sin_port;
        // Nao ta pegando a porta certa
        std::cout << "Aqui" << std::endl;
        std::cout << addr.address << ":" << addr.port << std::endl;
        return r;
    }
    else
    {
        std::cerr << "[SOCKET] Error receiving from socket" << std::endl;
        return -1;
    }
}