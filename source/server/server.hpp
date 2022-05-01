#pragma once
#include <inttypes.h>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include <Packet.hpp>
#include "PendingNotification.hpp"
#include "ProfileManager.hpp"
#include "ReplicaManager.hpp"
#include "SessionManager.hpp"
#include "Socket.hpp"

class Server
{
private:
    std::string bindAddress;
    int         bindPort;

    int       socketDescr;
    const int bufferSize = 2048;
    uint64_t  lastSeqn   = 0;

    Socket _socket;

    bool                            isListening                     = true;
    std::unique_ptr<std::thread>    listeningThread                 = nullptr;
    std::unique_ptr<std::thread>    pendingNotificationWorkerThread = nullptr;
    std::unique_ptr<std::thread>    heartBeatWorkerThread           = nullptr;
    std::vector<std::thread>        messageHandlerThreads;
    std::queue<PendingNotification> notificationQueue;

    std::mutex notificationQueueMutex;

    ProfileManager* profileManager;
    SessionManager* sessionManager;
    ReplicaManager* replicaManager;
    std::time_t     lastHeartbeatTimestamp;

    void Listen();
    void PendingNotificationWorker();
    void HeartbeatNotificationWorker();
    void ElectionAlgorithmProcess();
    void ParseInput(const char* buffer);
    void MessageHandler(Message::Packet message, SocketAddress sockAddr);
    void Reply(SocketAddress sockAddr, Message::Packet message);
    void Broadcast(Message::Packet message, Profile* exclude = nullptr);

public:
    Server(std::string bindAddress, int bindPort, std::string peersList, bool primary);

    Socket*  GetSocket();
    uint64_t GetLastSeqn() const { return lastSeqn; }
    void     IncrementSeqn() { lastSeqn++; }

    /* Inicia o servidor */
    void Start();

    /* Finaliza o servidor */
    void Stop();
};