#pragma once
#include <sstream>
#include <string>
#include <vector>
#include "../client/confirmation-buffer.hpp"
#include "Socket.hpp"

class Server;

static std::vector<std::string> Split(std::string a, const char delimiter)
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

typedef struct
{
    std::string address;
    int         port;
    bool        primary;

    SocketAddress GetSocketAddress()
    {
        SocketAddress addr;
        addr.address = address;
        addr.port    = port;
        return addr;
    }
} Peer;

class ReplicaManager
{
private:
    Peer thisPeer;

    /** Lista de hosts conectados na rede */
    std::vector<Peer> peers;

    std::unique_ptr<ConfirmationBuffer<3>> confirmationBuffer = nullptr;
    std::vector<std::string>               SplitPeerList(std::string peerList);

    std::vector<Peer> ParsePeersArgument(std::string peersStr);

    Server* server;

public:
    ReplicaManager(std::string address, int port, bool primary, std::string peerList,
                   Server* server);

    bool IsPrimary() const { return thisPeer.primary; }

    int ConfirmMessage(uint64_t seqn);

    /** Broadcasts a message to a group of peers */
    int BroadcastToSecondaries(Message::Packet message, SocketAddress senderAddress);

    int BroadcastHeartbeatToSecondaries(Message::Packet message);

    Server* GetServer() const { return server; }
    /** Returns a list of the secondary replicas in the network */
    std::vector<Peer> GetSecondaryReplicas();
    Peer              GetPrimaryReplica();
};