#include "ReplicaManager.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include "../client/confirmation-buffer.hpp"

ReplicaManager::ReplicaManager(std::string address, int port, bool primary, std::string peerList)
{
    thisPeer.address = address;
    thisPeer.port    = port;
    thisPeer.primary = primary;

    peers = ParsePeersArgument(peerList);

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
        if (peer.primary) r.push_back(peer);
    }

    return r;
}

int ReplicaManager::BroadcastToSecondaries()
{

    auto peers = GetSecondaryReplicas();
    for (auto& peer : peers)
    {
        std::cout << "Broadcasting to " << peer.address << ":" << peer.port << std::endl;
    }
}

int ReplicaManager::ConfirmMessage(uint64_t seqn)
{
    std::cout << "confirming message seqn=" << seqn << std::endl;
    if (IsPrimary())
    {
        //
        std::cout << "replica is primary" << std::endl;
    }
    else
    {
        std::cout << "replica is secondary" << std::endl;
    }
}