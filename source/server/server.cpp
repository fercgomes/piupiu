#pragma once

#include "server.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <string>

Server::Server(std::string bindAddress, int bindPort) : bindAddress(bindAddress), bindPort(bindPort)
{
    sessionManager = new SessionManager(new ProfileManager());

    // Inicializar socket
    // TODO: verificar return code do socket
    this->socketDescr = socket(AF_INET, SOCK_DGRAM, 0);
    std::cout << "Socket inicializado (" << this->socketDescr << ")" << std::endl;

    // Setar endereço do socket
    memset(&this->socketAddress, '\0', sizeof(this->socketAddress));
    this->socketAddress.sin_family      = AF_INET;
    this->socketAddress.sin_port        = htons(this->bindPort);
    this->socketAddress.sin_addr.s_addr = inet_addr(this->bindAddress.c_str());

    // Fazer o bind do socket
    // TODO: verificar return code
    int r = bind(this->socketDescr, (struct sockaddr*)&this->socketAddress,
                 sizeof(this->socketAddress));

    std::cout << "Socket bind (" << r << ")" << std::endl;
}

void Server::Listen()
{
    char               buffer[this->bufferSize];
    struct sockaddr_in incomingDataAddress;
    socklen_t          incomingDataAddressLength;

    while (this->isListening)
    {
        int r = recvfrom(this->socketDescr, buffer, this->bufferSize, 0,
                         (struct sockaddr*)&incomingDataAddress, &incomingDataAddressLength);

        printf("Received %d bytes from %s:%d\n", r, inet_ntoa(incomingDataAddress.sin_addr),
               incomingDataAddress.sin_port);

        if (r > 0)
        {
            std::cout << "Dispatching to message handler" << std::endl;
            messageHandlerThreads.push_back(
                std::thread(&Server::MessageHandler, this, std::string(buffer)));
        }
    }

    std::cout << "Stopping listening thread" << std::endl;
}

void Server::Start()
{
    this->listeningThread = std::make_unique<std::thread>(&Server::Listen, this);

    while (1)
        ;
}

void Server::Stop()
{
    close(this->socketDescr);
    std::cout << "Servidor finalizado." << std::endl;
}

void Server::MessageHandler(std::string message)
{
    std::thread::id thisId = std::this_thread::get_id();
    std::cout << "Handling message (threadId: " << thisId << ")" << std::endl;
    std::cout << message << std::endl;

    sessionManager->StartSession("Fernando");
    sessionManager->print();

    // Finalizando thread
    // TODO: remover thread da lista, ou se pa nem precisa de uma lista com as threads
}