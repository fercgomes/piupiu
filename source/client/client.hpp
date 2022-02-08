#pragma once

#include <string>
#include <thread>

class Client
{
private:
    std::string profileHandle;
    std::string serverAddress;
    int         serverPort;

    const size_t bufferSize = 1024;

    int socketDescr;

    bool                         isListening     = true;
    std::unique_ptr<std::thread> listeningThread = nullptr;

    void Listen();
    void SendMessageToServer(std::string message);

public:
    Client(std::string profile, std::string serverAddress, int serverPort);

    int  Connect();
    void Shutdown();

    int FollowUser(std::string profile);
    int Post(std::string recipient, std::string message);
};