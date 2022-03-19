#pragma once

#include "server.hpp"
#include <Packet.hpp>
#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include "PendingNotification.hpp"

uint64_t Server::lastSeqn;

Server::Server(std::string bindAddress, int bindPort) : bindAddress(bindAddress), bindPort(bindPort)
{
    profileManager = new ProfileManager();
    sessionManager = new SessionManager(profileManager);
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
            // Deveria limpar a lista periodicamente

            // TODO: talvez mapear a thread com o id de quem enviou
        }
        else
        {
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        }
    }

    std::cout << "Stopping listening thread" << std::endl;
}

void Server::PendingNotificationWorker()
{
    while (true)
    {
        const std::lock_guard<std::mutex> lock(notificationQueueMutex);

        if (!notificationQueue.empty())
        {
            // Pop notification from queue
            auto notification = notificationQueue.front();
            notificationQueue.pop();

            Profile* recipient = notification.recipient;
            Session* session   = recipient->GetSession();

            if (session && session->sockets.size() > 0)
            {
                std::cout << "Enviando notificação" << std::endl;
                for (auto socket : session->sockets)
                {
                    Reply(socket, Message::MakeNotification(lastSeqn, notification.body,
                                                            notification.senderUsername));
                }
            }
            else
            {
                // Retorna notificação pra fila
                notificationQueue.push(notification);
            }
        }
    }
}

void Server::Start()
{
    // Spawn listening thread
    this->listeningThread = std::make_unique<std::thread>(&Server::Listen, this);

    // Spawn notificationw worker thread
    this->pendingNotificationWorkerThread =
        std::make_unique<std::thread>(&Server::PendingNotificationWorker, this);

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
    std::thread::id   thisId = std::this_thread::get_id();
    std::stringstream ss;
    ss << inet_ntoa(sender.sin_addr) << ":" << sender.sin_port;
    std::string host = ss.str();

    std::cout << "Handling message (threadId: " << thisId << ")" << std::endl;
    // printf("type=%u\nseqn=%u\ntimestamp=%u\nlen=%u\npayload=%s\n", message.type, message.seqn,
    // message.timestamp, message.length, message.payload);

    switch (message.type)
    {
    case PACKET_CONNECT_CMD:
    {
        char*       user = message.payload;
        std::string username(message.payload);

        std::cout << "Connect request for " << username << " (" << host << ")." << std::endl;

        Profile* profile = profileManager->GetProfileByName(username);

        if (profile)
        { std::cout << "Profile for " << profile->GetHandle() << " found." << std::endl; }
        else
        {
            std::cout << "Profile for " << username << " not found." << std::endl;
            profile = profileManager->NewProfile(username);
        }

        auto session = sessionManager->StartSession(profile, sender);

        if (session)
        {
            std::cout << "Session for " << username << " created." << std::endl;
            profile->SetSession(session);

            // Accept connection
            Reply(sender, Message::MakeAcceptConnCommand(lastSeqn));

            // Show connected users
            auto              users = profileManager->GetConnectedUsers(profile);
            std::stringstream ss;
            for (auto user : users)
            {
                ss << user << ",";
            }
            std::string usersStr = "Connected users: " + ss.str();

            Reply(sender, Message::MakeInfo(lastSeqn, usersStr));

            // Broadcast connect notification
            Broadcast(Message::MakeInfo(lastSeqn, username + " has connected."), profile);
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
        char*       user = message.payload;
        std::string username(user);

        std::cout << "Disconnect request for " << username << " (" << host << ")." << std::endl;

        Profile* profile = profileManager->GetProfileByName(username);
        if (profile)
        {
            int ended = sessionManager->EndSession(profile, sender);

            // Broadcast connect notification
            Broadcast(Message::MakeInfo(lastSeqn, username + " has disconnected."), profile);

            std::cout << "Connections ended: " << ended << std::endl;
        }
        else
        {
            std::cout << "No profile found to disconnect" << std::endl;
            Reply(sender, Message::MakeError(lastSeqn, "Profile not found"));
        }

        break;
    }
    case PACKET_FOLLOW_CMD:
    {
        std::string usernameToFollow(message.payload);
        std::string username;

        if (sessionManager->GetUserNameByAddressAndIP(sender.sin_addr, sender.sin_port, username))
        {
            Profile* profile = profileManager->GetProfileByName(usernameToFollow);

            if (usernameToFollow.compare(username) == 0)
            {
                Reply(sender, Message::MakeError(lastSeqn, "You can't follow yourself"));
                return;
            }

            if (profile)
            {
                if (!profile->AddFollower(username))
                {
                    Reply(sender,
                          Message::MakeError(lastSeqn, "You're already following this user"));
                }
            }
            else
            {
                std::cerr << "Profile " << usernameToFollow << " not found." << std::endl;
                Reply(sender, Message::MakeError(lastSeqn, "Profile not found"));
            }
        }
        else
        {
            std::cerr << "You're not authenticated" << std::endl;
            Reply(sender, Message::MakeError(lastSeqn, "Not authenticated"));
        }
        break;
    }
    case PACKET_SEND_CMD:
    {
        std::string username;

        if (sessionManager->GetUserNameByAddressAndIP(sender.sin_addr, sender.sin_port, username))
        {
            std::cout << "Send message request from " << username << " (" << host << ")."
                      << std::endl;

            Profile* profile = profileManager->GetProfileByName(username);
            if (profile)
            {
                auto followers = profile->GetFollowers();
                for (auto follower : followers)
                {
                    std::string         body(message.payload);
                    PendingNotification notification(username, body, follower);

                    {
                        const std::lock_guard<std::mutex> lock(notificationQueueMutex);
                        notificationQueue.push(notification);
                    }
                }
            }
            else
            {
                std::cerr << host << " is not authenticated." << std::endl;
                Reply(sender, Message::MakeError(lastSeqn, "Not authenticated"));
            }
        }
        else
        {
            std::cerr << host << " is not authenticated." << std::endl;
            Reply(sender, Message::MakeError(lastSeqn, "Not authenticated"));
        }
        break;
    }
    default:
        std::cerr << "Server should receive this message type" << std::endl;
        Reply(sender, Message::MakeError(lastSeqn, "Invalid command"));
        break;
    }

    // Finalizando thread
    // TODO: remover thread da lista, ou se pa nem precisa de uma lista com as threads
    // auto it = std::find_if(messageHandlerThreads.begin(), messageHandlerThreads.end(),
    //                        [thisId](std::thread& t) { return t.get_id() == thisId; });

    // if (it != messageHandlerThreads.end()) { messageHandlerThreads.erase(it); }
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

void Server::Broadcast(Message::Packet message, Profile* exclude)
{
    auto sockets = profileManager->GetConnectedSockets(exclude);
    for (auto socket : sockets)
    {
        Reply(socket, message);
    }
}