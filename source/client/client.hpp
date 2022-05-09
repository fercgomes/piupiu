#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "confirmation-buffer.hpp"
#include "console/message.hpp"

#include <Packet.hpp>

class Client
{
private:
    std::string profileHandle;
    std::string serverAddress;
    int         serverPort;

    const size_t bufferSize = 1024;

    int             socketDescr;
    static uint64_t lastSentSeqn;

    bool connected   = false;
    bool isListening = true;

    std::unique_ptr<std::thread> listeningThread = nullptr;
    std::unique_ptr<std::thread> reorderThread   = nullptr;

    std::mutex packetQueueMutex;

    // Lista para recebimento dos packets caso necessario
    std::vector<Message::Packet> PacketList;

    //
    using HandlerFn = std::function<void(std::string, enum MessageType)>;
    HandlerFn messageHandler;

    void Listen();
    void Reorder();

    int SendMessageToServer(Message::Packet message);

public:
    Client(std::string profile, std::string serverAddress, int serverPort);
    Client();

    int Connect();
    int Connect(std::string profile, std::string serverAddress, int serverPort);

    void Shutdown();

    int FollowUser(std::string profile);
    int Post(std::string message);
    int GetInfo();

    void        SetProfileHandle(std::string profileHandle);
    void        SetServerAddress(std::string serverAddress);
    void        SetServerPort(int port);
    void        SetMessageHandlerFunc(HandlerFn func);
    std::string GetHostStr();

    void HelpInfo();

    bool IsConnected() const;
};