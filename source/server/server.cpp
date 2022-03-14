#pragma once

#include "server.hpp"
#include <Packet.hpp>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <string>

uint64_t Server::lastSeqn;

Server::Server(std::string bindAddress, int bindPort) : bindAddress(bindAddress), bindPort(bindPort)
{
    sessionManager = new SessionManager(new ProfileManager());
    lastSeqn       = 0;

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
    if (r < 0) { fprintf(stderr, "bind() failed: %s\n", strerror(errno)); }
}

void Server::Listen()
{
    char               buffer[this->bufferSize];
    struct sockaddr_in incomingDataAddress;
    socklen_t          incomingDataAddressLength = sizeof(incomingDataAddress);

    // Main listening loop
    // Each message that is received gets passe to a message handler.
    // The handler parses the message and executes it.
    while (this->isListening)
    {
        int r = recvfrom(this->socketDescr, buffer, this->bufferSize, 0,
                         (struct sockaddr*)&incomingDataAddress, &incomingDataAddressLength);

        // printf("Received %d bytes from %s:%d\n", r, inet_ntoa(incomingDataAddress.sin_addr),
        //        incomingDataAddress.sin_port);

        if (r > 0)
        {
            std::cout << "Dispatching to message handler" << std::endl;

            auto msg = reinterpret_cast<Message::Packet*>(buffer);

            messageHandlerThreads.push_back(
                std::thread(&Server::MessageHandler, this, *msg, incomingDataAddress));

            // TODO: talvez mapear a thread com o id de quem enviou
        }
        else
        {
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));
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

void Server::MessageHandler(Message::Packet message, struct sockaddr_in sender)
{
    std::thread::id thisId = std::this_thread::get_id();
    std::cout << "Handling message (threadId: " << thisId << ")" << std::endl;
    // printf("type=%u\nseqn=%u\ntimestamp=%u\nlen=%u\npayload=%s\n", message.type, message.seqn,
    // message.timestamp, message.length, message.payload);

    switch (message.type)
    {
    case PACKET_CONNECT_CMD:
    {
        char* user = message.payload;
        char  username[100];
        memset(username, 0, 100);
        strcpy(username, message.payload);

        auto ret = sessionManager->StartSession(std::string(username), sender);
        if (ret)
        {
            std::cout << username << " connected" << std::endl;
            Reply(sender, Message::MakeAcceptConnCommand(lastSeqn));
        }
        else
        {
            std::cout << "Max connection reach for " << username << std::endl;
            Reply(sender, Message::MakeRejectConnCommand(lastSeqn));
        }

        break;
    }
    case PACKET_DISCONNECT_CMD:
    {
        char* user = message.payload;
        printf("Disconnect user %s\n", user);
        int ended = sessionManager->EndSession(std::string(user), sender);
        printf("Connections ended: %d\n", ended);

        break;
    }
    case PACKET_ACCEPT_CONN_CMD:
    {
        std::cout << "Accept conn message received" << std::endl;
        break;
    }
    case PACKET_REJECT_CONN_CMD:
    {
        std::cout << "Reject conn message received" << std::endl;
        break;
    }
    case PACKET_FOLLOW_CMD:
    {
        char* username = message.payload;
        printf("Follow %s\n", message.payload);
        break;
    }
    case PACKET_SEND_CMD:
    {
        std::cout << "Send message received" << std::endl;
        printf("Send message: %s\n", message.payload);
        Reply(sender, Message::MakeNotification(lastSeqn, std::string(message.payload),
                                                std::string("fernando")));
        break;
    }
    }

    // Finalizando thread
    // TODO: remover thread da lista, ou se pa nem precisa de uma lista com as threads
}

void Server::Reply(struct sockaddr_in sender, Message::Packet message)
{

    int r = sendto(this->socketDescr, &message, sizeof(message), 0, (struct sockaddr*)&sender,
                   sizeof(sender));

    if (r < 0) { fprintf(stderr, "sendto() failed: %s\n", strerror(errno)); }
    else
    {
        printf("Replied to client succesfully\n");
    }
}