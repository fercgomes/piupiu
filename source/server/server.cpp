#pragma once

#include "server.hpp"
#include <Packet.hpp>
#include <algorithm>
#include <arpa/inet.h>
#include <ctime>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include "PendingNotification.hpp"

static std::vector<std::string> Split2(std::string a, const char delimiter)
{
    std::stringstream        ss(a);
    std::string              str;
    std::vector<std::string> results;

    while (std::getline(ss, str, delimiter))
    {
        results.push_back(str);
    }

    return results;
};

Server::Server(std::string bindAddress, int bindPort, std::string peersList, bool primary)
    : bindAddress(bindAddress), bindPort(bindPort)
{
    replicaManager = new ReplicaManager(bindAddress, bindPort, primary, peersList, this);
    profileManager = new ProfileManager(this);
    sessionManager = new SessionManager(profileManager);
    lastSeqn       = 0;

    // Inicializar socket
    _socket.Bind(bindAddress, bindPort);

    // this->socketDescr = socket(AF_INET, SOCK_DGRAM, 0);
    // std::cout << "Socket inicializado (" << this->socketDescr << ")" << std::endl;

    // // Setar endereço do socket
    // memset(&this->socketAddress, '\0', sizeof(this->socketAddress));
    // this->socketAddress.sin_family      = AF_INET;
    // this->socketAddress.sin_port        = htons(this->bindPort);
    // this->socketAddress.sin_addr.s_addr = inet_addr(this->bindAddress.c_str());

    // // Fazer o bind do socket
    // int r = bind(this->socketDescr, (struct sockaddr*)&this->socketAddress,
    //              sizeof(this->socketAddress));

    // std::cout << "Socket bind (" << r << ")" << std::endl;
    // if (r < 0) { fprintf(stderr, "bind() failed: %s\n", strerror(errno)); }
}

void Server::Listen()
{
    char               buffer[this->bufferSize];
    struct sockaddr_in incomingDataAddress;
    socklen_t          incomingDataAddressLength = sizeof(incomingDataAddress);
    std::string        incAddr;
    int                incPort;

    // Main listening loop
    // Each message that is received gets passe to a message handler.
    // The handler parses the message and executes it.
    std::cout << "Listening to messages" << std::endl;
    while (this->isListening)
    {
        // int r = recvfrom(this->socketDescr, buffer, this->bufferSize, 0,
        //                  (struct sockaddr*)&incomingDataAddress, &incomingDataAddressLength);
        SocketAddress incomingAddress;
        int           r = _socket.Receive(buffer, bufferSize, incomingAddress);

        // printf("Received %d bytes from %s:%d\n", r, inet_ntoa(incomingDataAddress.sin_addr),
        //        incomingDataAddress.sin_port);

        if (r > 0)
        {
            std::cout << "Dispatching to message handler" << std::endl;

            auto msg = reinterpret_cast<Message::Packet*>(buffer);

            messageHandlerThreads.push_back(
                std::thread(&Server::MessageHandler, this, *msg, incomingAddress));
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
                    Reply(socket, Message::MakeNotification(++lastSeqn, notification.body,
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

void Server::ElectionAlgorithmProcess()
{
    electionStarted = true;
    // Current time
    lastElectionTimestamp = std::time(nullptr);

    // Let's pick up all the secondary replicas
    std::vector<Peer> secondary_connections = replicaManager->GetSecondaryReplicas();

    // Fetch peers using port as a parameter
    for (auto i = secondary_connections.begin(); i != secondary_connections.end(); i++)
    {
        if (this->bindPort < i->port)
        {
            // TODO make new SocketAddress
            SocketAddress peerAddr = SocketAddress(this->bindAddress, this->bindPort);
            Reply(peerAddr, Message::MakeElection(++lastSeqn));
        }
    }
}

void Server::HeartbeatNotificationWorker()
{
    while (true)
    {
        if (replicaManager->IsPrimary())
        {
            sleep(4);
            replicaManager->BroadcastHeartbeatToSecondaries(Message::MakeHeartbeatMessage());
        }
        else
        {
            sleep(4);
            if (std::time(nullptr) - lastHeartbeatTimestamp > 9 && electionStarted == false)
            {
                std::cout << "Timeout from heartbeats. Starting election" << std::endl;
                ElectionAlgorithmProcess();
            }
            else
            {
                std::cout << "Nothing to do here" << std::endl;
            }
        }
    }
}

std::string Server::GetIpAddr() { return bindAddress; }

int Server::GetPort() { return bindPort; }

void Server::ElectionTimeoutWorker()
{
    while (true)
    {
        if (!(replicaManager->IsPrimary()))
        {
            sleep(1);
            if (std::time(nullptr) - lastElectionTimestamp > 5 && electionStarted == true)
            {
                std::cout << "I got no response of my election." << std::endl;
                std::cout << "Starting bully. I'm the new coordinator" << std::endl;
                std::cout << lastElectionTimestamp << std::endl;
                std::cout << std::time(nullptr) << std::endl;
                std::vector<Peer> peers = replicaManager->GetSecondaryReplicas();

                for (auto peer = peers.begin(); peer != peers.end(); peer++)
                {
                    SocketAddress peerAddr = peer->GetSocketAddress();
                    Reply(peerAddr, Message::Coordinator(++lastSeqn, bindAddress, bindPort));
                }
                // Deletar o atual primary
                replicaManager->DeletePrimaryReplica();

                for (auto peer = peers.begin(); peer != peers.end(); peer++)
                {
                    SocketAddress peerAddr = peer->GetSocketAddress();
                    Reply(peerAddr, Message::Coordinator(++lastSeqn, bindAddress, bindPort));
                }
                // Deletar o atual primary
                replicaManager->DeletePrimaryReplica();

                // Como as fun��es de session manager pegam o IP e Porta usando
                // a fun��o GetPrimaryReplica, o que precisamos fazer entao
                //� transformar a secundaria eleita em primaria
                std::cout << "Making myself as primary replica" << std::endl;
                replicaManager->MakePrimaryReplica();

                // Retransmitir o endere�o e porta para os clients conectados
                std::vector<Session*> clients_connected = sessionManager->GetSessions();
                std::cout << "=========================" << std::endl;
                std::cout << "sessions " << clients_connected.size() << std::endl;

                for (auto client = clients_connected.begin(); client != clients_connected.end();
                     client++)
                {
                    // Transmite o endere�o do novo primario aos clients
                    // SocketAddress clientAddr = SocketAddress(client.bindAddress, client.bindPort);
                    auto a = *client;
                    std::cout << a->profile << std::endl;
                    std::vector<SocketAddress> client_sockets = (*client)->sockets;
                    for (auto socket_cli = client_sockets.begin();
                         socket_cli != client_sockets.end(); socket_cli++)
                    {
                        // Transmite o endere�o do novo primario aos clients
                        // SocketAddress clientAddr = SocketAddress(client.bindAddress,
                        // client.bindPort);
                        SocketAddress clientTransmission = *socket_cli;
                        std::cout << "sending to " << clientTransmission.address << ":"
                                  << clientTransmission.port << std::endl;

                        _socket.Send(
                            clientTransmission,
                            Message::Coordinator(++lastSeqn, this->GetIpAddr(), this->GetPort()));
                    }
                }
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

    // Spawn heartbeat worker thread
    this->heartBeatWorkerThread =
        std::make_unique<std::thread>(&Server::HeartbeatNotificationWorker, this);

    // Spawn election worker thread
    this->electionTimeoutWorkerThread =
        std::make_unique<std::thread>(&Server::ElectionTimeoutWorker, this);

    while (1)
        ;
}

void Server::Stop()
{
    close(this->socketDescr);
    std::cout << "Servidor finalizado." << std::endl;
}

void Server::MessageHandler(Message::Packet message, SocketAddress incomingAddress)
{
    std::thread::id   thisId = std::this_thread::get_id();
    std::stringstream ss;
    ss << incomingAddress.address << ":" << incomingAddress.port;
    std::string host = ss.str();

    std::cout << "Handling message (threadId: " << thisId
              << ") type=" << Message::TypeToStr(message.type) << std::endl;
    // printf("type=%u\nseqn=%u\ntimestamp=%u\nlen=%u\npayload=%s\n", message.type, message.seqn,
    // message.timestamp, message.length, message.payload);

    switch (message.type)
    {
    case PACKET_CONNECT_CMD:
    {
        char*       user = message.payload;
        std::string username(message.payload);

        std::cout << "Connect request for " << username << " (" << host << ")." << std::endl;

        // Do state change here

        Profile* profile = profileManager->GetProfileByName(username);

        if (profile)
        { std::cout << "Profile for " << profile->GetHandle() << " found." << std::endl; }
        else
        {
            std::cout << "Profile for " << username << " not found." << std::endl;
            profile = profileManager->NewProfile(username);
        }

        auto session = sessionManager->StartSession(profile, incomingAddress);

        if (session)
        {
            std::cout << "Session for " << username << " created." << std::endl;
            profile->SetSession(session);

            // Accept connection
            // Se for primario

            // Retransmitir o pacote para cada secundario
            // Enviar para N secundários

            // Aguardar N confirmações

            // Quando todos os secundarios confirmarem, confirmar essa mensagem.
            // Reply(incomingAddress, Message::MakeAcceptConnCommand(++lastSeqn));

            // Recebeu as N confirmações
            // Confirma de volta pro client
            // === Termina primario ===

            // Se for secundario
            // Confirma de volta pro servidor primario.
            // === Termina secundario ===

            // Show connected users
            auto              users = profileManager->GetConnectedUsers(profile);
            std::stringstream ss;
            for (auto user : users)
            {
                ss << user << ",";
            }
            std::string usersStr = "Connected users: " + ss.str();
            // Reply(incomingAddress, Message::MakeInfo(++lastSeqn, usersStr));

            // Broadcast connect notification
            // Broadcast(Message::MakeInfo(++lastSeqn, username + " has connected."), profile);
        }
        else
        {
            std::cout << "Max connection reach for " << username << std::endl;
            // Reply(incomingAddress, Message::MakeRejectConnCommand(++lastSeqn));
        }

        if (replicaManager->IsPrimary())
        {
            std::cout << "[connect user] primary" << std::endl;

            // broadcast state change to secondaries
            // Reply(incomingAddress, Message::MakeAcceptConnCommand(++lastSeqn));
            replicaManager->BroadcastToSecondaries(message, incomingAddress);
            std::cout << "debug dos guris funcionando" << std::endl;
        }
        else
        {
            // confirm state change back to primary

            std::cout << "[connect user] secondary" << std::endl;
            replicaManager->ConfirmMessage(message.seqn);
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
            int ended = sessionManager->EndSession(profile, incomingAddress);

            // Broadcast connect notification
            // Broadcast(Message::MakeInfo(++lastSeqn, username + " has disconnected."), profile);

            std::cout << "Connections ended: " << ended << std::endl;
        }
        else
        {
            std::cout << "No profile found to disconnect" << std::endl;
            if (replicaManager->IsPrimary())
            {
                replicaManager->BroadcastToSecondaries(message, incomingAddress);
                Reply(incomingAddress, Message::MakeError(++lastSeqn, "Profile not found"));
            }
            else
            {
                std::cout << "[disconnect] secondary" << std::endl;
                replicaManager->ConfirmMessage(message.seqn);
            }
        }

        break;
    }
    case PACKET_FOLLOW_CMD:
    {
        // std::string usernameToFollow(message.payload);
        auto        splits           = Split2(std::string(message.payload), '\n');
        std::string username         = splits[0];
        std::string usernameToFollow = splits[1];

        // if (sessionManager->GetUserNameByAddressAndIP(incomingAddress, username))
        // {
        std::cout << "[FOLLOW] Found session" << std::endl;
        Profile* profile = profileManager->GetProfileByName(usernameToFollow);

        if (usernameToFollow.compare(username) == 0)
        {
            std::cout << "user trying to follow himself" << std::endl;
            if (replicaManager->IsPrimary())
            {
                Reply(incomingAddress, Message::MakeError(++lastSeqn, "You can't follow yourself"));
            }
            return;
        }

        if (profile)
        {
            std::cout << "[FOLLOW] Found user" << std::endl;
            if (!profile->AddFollower(username))
            {
                std::cout << "Already following this user " << std::endl;
                // Reply(incomingAddress,
                //       Message::MakeError(++lastSeqn, "You're already following this user"));
            }
            else
            {
                if (replicaManager->IsPrimary())
                {
                    std::cout << "[Primary] following " << usernameToFollow << std::endl;
                    // Reply(incomingAddress, Message::MakeInfo(++lastSeqn, "You're following "
                    // +
                    //                                                          usernameToFollow));

                    replicaManager->BroadcastToSecondaries(message, incomingAddress);
                }
                else
                {
                    std::cout << "[follow] secondary - You're following" + usernameToFollow
                              << std::endl;
                    replicaManager->ConfirmMessage(message.seqn);
                }
            }
        }
        else
        {
            std::cout << "Profile " << usernameToFollow << " not found." << std::endl;
            // Reply(incomingAddress, Message::MakeError(++lastSeqn, "Profile not found"));
        }
        // }
        // else
        // {
        //     std::cout << "You're not authenticated" << std::endl;
        //     // Reply(incomingAddress, Message::MakeError(++lastSeqn, "Not authenticated"));
        // }
        break;
    }
    case PACKET_SEND_CMD:
    {
        std::string username;

        if (sessionManager->GetUserNameByAddressAndIP(incomingAddress, username))
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
                Reply(incomingAddress, Message::MakeInfo(++lastSeqn, "Message sent."));
            }
            else
            {
                std::cerr << host << " is not authenticated." << std::endl;
                // Reply(incomingAddress, Message::MakeError(++lastSeqn, "Not authenticated"));
            }
        }
        else
        {
            std::cerr << host << " is not authenticated." << std::endl;
            // Reply(incomingAddress, Message::MakeError(++lastSeqn, "Not authenticated"));
        }
        break;
    }
    case PACKET_REQUEST_USER_INFO:
    {
        std::string       username;
        std::string       response;
        std::stringstream ss;

        if (sessionManager->GetUserNameByAddressAndIP(incomingAddress, username))
        {
            Profile* profile = profileManager->GetProfileByName(username);
            if (profile)
            {
                auto users = profileManager->GetConnectedUsers(profile);

                ss << "\nUsername: " << profile->GetHandle();
                ss << "\nFollowers: " << profile->GetFollowers().size();

                if (users.size() > 0)
                {
                    ss << "\nConnected users: \n";
                    for (auto user : users)
                    {
                        ss << user << ",";
                    }
                    ss << "\n";
                }
                else
                {
                    ss << "\nNo users connected.\n";
                }

                response = ss.str();
                // Reply(incomingAddress, Message::MakeInfo(++lastSeqn, response));
            }
        }

        break;
    }
    case PACKET_CONFIRM_STATE_CHANGE:
    {
        std::cout << "Received confirm state change message" << std::endl;
        replicaManager->ConfirmMessage(message.seqn);
        break;
    }
    case PACKET_HEARTBEAT:
    {
        if (replicaManager->IsPrimary())
        {
            std::cerr << "I'm primary. Why am I receibing heartbeat message?" << std::endl;
            break;
        }
        else
        {
            std::cout << "Primary sent me a heartbeat. I will still be secondary" << std::endl;
            lastHeartbeatTimestamp = std::time(nullptr);
            lastElectionTimestamp  = std::time(nullptr);
        }
        break;
    }
    case PACKET_REPLY:
    {
        electionStarted = false;
        // If a reply was received
        std::cerr << host << "I got an answer from an election process. Aborting my own election!!!"
                  << std::endl;
        break;
    }
    case PACKET_COORDINATOR:
    {
        std::vector<Peer> avaliable_peers = replicaManager->GetPeersList();
        for (auto peer = avaliable_peers.begin(); peer != avaliable_peers.end(); peer++)
        {
            Peer currentPeer = *peer;
            if ((!currentPeer.address.compare(incomingAddress.address)) &&
                (currentPeer.port == incomingAddress.port))
            {
                currentPeer.primary = true;
                std::cerr << host << "Updating new coordinator!!!" << std::endl;
            }
            else
            {
                currentPeer.primary = false;
            }
        }
        std::cerr << host << "There is a new cordinator in the house!!!" << std::endl;
        break;
    }
    case PACKET_ELECTION:
    {
        Reply(incomingAddress,
              Message::MakeReply(++lastSeqn,
                                 "There is an election going on. Starting my own election"));
        if (!electionStarted) { ElectionAlgorithmProcess(); }
        break;
    }
    default:
        std::cerr << "Server should not receive this message type" << std::endl;
        // Reply(incomingAddress, Message::MakeError(++lastSeqn, "Invalid command"));
        break;
    }

    // Finalizando thread
    // TODO: remover thread da lista, ou se pa nem precisa de uma lista com as threads
    // auto it = std::find_if(messageHandlerThreads.begin(), messageHandlerThreads.end(),
    //                        [thisId](std::thread& t) { return t.get_id() == thisId; });

    // if (it != messageHandlerThreads.end()) { messageHandlerThreads.erase(it); }
}

void Server::Reply(SocketAddress address, Message::Packet message)
{

    int r = _socket.Send(address, message);

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

Socket* Server::GetSocket() { return &_socket; }