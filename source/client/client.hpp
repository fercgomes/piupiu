#pragma once

#include <string>
#include <thread>

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

    bool                         connected       = false;
    bool                         isListening     = true;
    std::unique_ptr<std::thread> listeningThread = nullptr;
    std::unique_ptr<std::thread> reorderThread = nullptr;

    std::mutex packetQueueMutex;

    //Lista para recebimento dos packets caso necessario
    std::list<Packet> PacketList;

    //Fila para ordenar os packets pelo numero de sequencia
    std::queue<Packet> PacketQueue;

    void Listen();
    void Reorder();

    int  SendMessageToServer(Message::Packet message);

public:
    Client(std::string profile, std::string serverAddress, int serverPort);
    Client();

    int Connect();
    int Connect(std::string profile, std::string serverAddress, int serverPort);

    void Shutdown();

    int FollowUser(std::string profile);
    int Post(std::string message);
    int Info();

    void SetProfileHandle(std::string profileHandle);
    void SetServerAddress(std::string serverAddress);
    void SetServerPort(int port);

    void HelpInfo();

    bool IsConnected() const;
};