#include "ReplicaManager.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include "../client/confirmation-buffer.hpp"
#include "../messaging/Packet.hpp"
#include "Socket.hpp"
#include "server.hpp"

ReplicaManager::ReplicaManager(std::string address, int port, bool primary, std::string peerList,
                               Server* server)
{
    thisPeer.address = address;
    thisPeer.port    = port;
    thisPeer.primary = primary;

    peers = ParsePeersArgument(peerList);

    this->server = server;

    if (primary)
    {
        confirmationBuffer = std::make_unique<ConfirmationBuffer<2>>(
            [=](ConfirmationBuffer<2>::ItemType& container) {
                std::cout << "Inside callback, original seqn " << container.originalSeqn
                          << std::endl;

                // BUG: tem que confirmar pro client original
                // Aqui tá mandando sempre pro primary

                auto message = container.content[0].item->GetPacket();
                switch (message.type)
                {
                case PACKET_CONNECT_CMD:
                    this->GetServer()->GetSocket()->Send(
                        container.originalHost,
                        Message::MakeAcceptConnCommand(container.originalSeqn));
                    break;

                    // TODO: Adicionar as outras confirmaçoes

                case PACKET_SEND_CMD:
                {
                    this->GetServer()->GetSocket()->Send(
                        container.originalHost,
                        Message::MakeNotification(message.seqn, message.payload,
                                                  container.originalHost.address));
                }

                default:
                    std::cout << "ops" << std::endl;
                    break;
                }

                for (auto& item : container.content)
                {
                    std::cout << "seqn " << item.item->GetSequenceNumber() << " was confirmed"
                              << std::endl;
                }
            });
    }
};

std::vector<std::string> ReplicaManager::SplitPeerList(std::string peerList)
{
    return Split(peerList, ',');
};

std::vector<Peer> ReplicaManager::ParsePeersArgument(std::string peersStr)
{
    std::vector<Peer> peers;

    try
    {
        std::vector<std::string> peersList = Split(peersStr, ',');
        peers.reserve(peersList.size());

        for (auto& peer : peersList)
        {

            std::vector<std::string> parsed  = Split(peer, ':');
            std::string              ip      = parsed[0];
            int                      port    = atoi(parsed[1].c_str());
            bool                     primary = strcmp(parsed[2].c_str(), "p") == 0 ? true : false;

            Peer p;
            p.address = ip;
            p.port    = port;
            p.primary = primary;
            std::cout << ip << ", " << port << ", " << primary << std::endl;

            peers.push_back(p);
        }
    }
    catch (const char* e)
    {
        std::cerr << "Malformed peers argument" << std::endl;
        std::cerr << e << std::endl;
    }

    return peers;
}
std::vector<Peer> ReplicaManager::GetSecondaryReplicas()
{
    std::vector<Peer> r;
    for (auto peer : peers)
    {
        if (!peer.primary) r.push_back(peer);
    }

    return r;
}

Peer ReplicaManager::GetPrimaryReplica()
{
    if (IsPrimary()) return thisPeer;
    std::cout << "[GetPrimaryReplica]" << std::endl;

    for (auto peer : peers)
    {
        std::cout << peer.address << ":" << peer.port << " "
                  << (peer.primary ? "primary" : "secondary") << std::endl;

        if (peer.primary) return peer;
    }

    std::cerr << "No primary peer found" << std::endl;
    throw std::invalid_argument("No primary peer found");
}

void ReplicaManager::MakePrimaryReplica()
{
    thisPeer.primary = true;

    confirmationBuffer =
        std::make_unique<ConfirmationBuffer<2>>([=](ConfirmationBuffer<2>::ItemType& container) {
            std::cout << "Inside callback, original seqn " << container.originalSeqn << std::endl;

            // BUG: tem que confirmar pro client original
            // Aqui tá mandando sempre pro primary

            auto message = container.content[0].item->GetPacket();
            switch (message.type)
            {
            case PACKET_CONNECT_CMD:
                this->GetServer()->GetSocket()->Send(
                    container.originalHost, Message::MakeAcceptConnCommand(container.originalSeqn));
                break;

                // TODO: Adicionar as outras confirmaçoes

            case PACKET_SEND_CMD:
            {
                this->GetServer()->GetSocket()->Send(
                    container.originalHost,
                    Message::MakeNotification(message.seqn, message.payload,
                                              container.originalHost.address));
            }

            default:
                std::cout << "ops" << std::endl;
                break;
            }

            for (auto& item : container.content)
            {
                std::cout << "seqn " << item.item->GetSequenceNumber() << " was confirmed"
                          << std::endl;
            }
        });

    // Assume que quando rodar isso é pq uma replica caiu
    confirmationBuffer->IncOffset();
}

std::vector<Peer>* ReplicaManager::GetPeersList() { return &peers; }

void ReplicaManager::DeletePrimaryReplica()
{
    std::cout << "[DeletePrimaryReplica]" << std::endl;

    for (auto it = peers.begin(); it != peers.end(); it++)
    {
        std::cout << it->address << ":" << it->port << " " << it->primary << std::endl;
        if (it->primary)
        {
            peers.erase(it);
            return;
        }
    }
}

int ReplicaManager::BroadcastToSecondaries(Message::Packet message, SocketAddress senderAddress)
{
    auto peers = GetSecondaryReplicas();

    uint64_t lastSeqn     = server->GetLastSeqn();
    uint64_t lastSeqnTemp = server->GetLastSeqn();

    // uint64_t lastSeqn = 0;

    // Gambiarra!!
    std::array<BaseMessage*, 2> messages = {new BaseMessage(lastSeqn++, message),
                                            new BaseMessage(lastSeqn++, message)};

    for (int i = 0; i < 3; i++)
        server->IncrementSeqn();

    std::cout << "Last seqn in conf buffer: " << lastSeqn << std::endl;
    std::cout << "Last seqn in server: " << server->GetLastSeqn() << std::endl;

    confirmationBuffer->Push(messages, message.seqn, senderAddress);

    // Se nao tiver 3 peers, vai dar ruim
    for (int i = 0; i < peers.size(); i++)
    {
        auto peer = peers[i];
        std::cout << "Broadcasting message with seqn=" << message.seqn << " to " << peer.address
                  << ":" << peer.port << std::endl;
        SocketAddress addr;
        addr.address = peer.address;
        addr.port    = peer.port;

        // TODO: Precisa criar novos números de sequencia
        Message::Packet newMsg = message;
        message.seqn           = lastSeqnTemp + i;

        server->GetSocket()->Send(addr, message);
    }
}

int ReplicaManager::BroadcastHeartbeatToSecondaries(Message::Packet message, int currentPort)
{
    auto peers = GetSecondaryReplicas();

    uint64_t lastSeqn     = server->GetLastSeqn();
    uint64_t lastSeqnTemp = server->GetLastSeqn();

    // uint64_t lastSeqn = 0;

    // Gambiarra!!
    std::array<BaseMessage*, 3> messages = {
        new BaseMessage(lastSeqn++), new BaseMessage(lastSeqn++), new BaseMessage(lastSeqn++)};

    // Se nao tiver 3 peers, vai dar ruim
    for (int i = 0; i < peers.size(); i++)
    {
        auto peer = peers[i];
        // std::cout << "Broadcasting heartbeat"
        //           << " to " << peer.address << ":" << peer.port << std::endl;
        SocketAddress addr;
        addr.address = peer.address;
        addr.port    = peer.port;

        // TODO: Precisa criar novos números de sequencia
        Message::Packet newMsg = message;
        message.seqn           = lastSeqnTemp + i;

        if (addr.port < currentPort) { server->GetSocket()->Send(addr, message); }
    }
}

int ReplicaManager::ConfirmMessage(uint64_t seqn)
{
    std::cout << "confirming message seqn=" << seqn << std::endl;
    if (IsPrimary())
    {
        //
        std::cout << "replica is primary, confirming to buffer seqn " << seqn << std::endl;
        confirmationBuffer->Confirm(seqn);
    }
    else
    {
        auto primaryReplica = GetPrimaryReplica().GetSocketAddress();

        std::cout << "replica is secondary" << std::endl;
        std::cout << "confirming seqn " << seqn << " to " << primaryReplica.address << ":"
                  << primaryReplica.port << std::endl;
        // Send confirm message back

        server->GetSocket()->Send(primaryReplica, Message::MakeConfirmStateChangeMessage(seqn));
    }
}