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
        confirmationBuffer =
            std::make_unique<ConfirmationBuffer<3>>([](ConfirmationBuffer<3>::ItemType& container) {
                std::cout << "Inside callback, original seqn " << container.originalSeqn
                          << std::endl;
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

    for (auto peer : peers)
    {
        if (peer.primary) return peer;
    }

    std::cerr << "No primary peer found" << std::endl;
    throw std::invalid_argument("No primary peer found");
}

int ReplicaManager::BroadcastToSecondaries(Message::Packet message)
{
    auto peers = GetSecondaryReplicas();

    uint64_t lastSeqn = server->GetLastSeqn();
    // uint64_t lastSeqn = 0;

    // Gambiarra!!
    std::array<BaseMessage*, 3> messages = {
        new BaseMessage(lastSeqn++), new BaseMessage(lastSeqn++), new BaseMessage(lastSeqn++)};

    for (int i = 0; i < 3; i++)
        server->IncrementSeqn();

    std::cout << "Last seqn in conf buffer: " << lastSeqn << std::endl;
    std::cout << "Last seqn in server: " << server->GetLastSeqn() << std::endl;

    confirmationBuffer->Push(messages);

    // Se nao tiver 3 peers, vai dar ruim
    for (auto& peer : peers)
    {
        std::cout << "Broadcasting message with seqn=" << message.seqn << " to " << peer.address
                  << ":" << peer.port << std::endl;
        SocketAddress addr;
        addr.address = peer.address;
        addr.port    = peer.port;

        // TODO: Precisa criar novos números de sequencia
        Message::Packet newMsg = message;

        server->GetSocket()->Send(addr, message);
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