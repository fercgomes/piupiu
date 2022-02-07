#pragma once

#include <string>
#include <thread>

class Client
{
private:
    std::string profileHandle;
    std::string serverAddress;
    int         serverPort;

    int socketDescr;

    bool                         isListening     = true;
    std::unique_ptr<std::thread> listeningThread = nullptr;

    void Listen();
    void Shutdown();

public:
    Client(std::string profile, std::string serverAddress, int serverPort);

    int Connect();

    int FollowUser(std::string profile);
    int SendMessage(std::string recipient, std::string message);
};