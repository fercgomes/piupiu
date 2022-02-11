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
    int  SendMessageToServer(std::string message);

public:
    Client(std::string profile, std::string serverAddress, int serverPort);
    Client();

    int Connect();
    int Connect(std::string profile, std::string serverAddress, int serverPort);

    void Shutdown();

    int FollowUser(std::string profile);
    int Post(std::string message);

    void SetProfileHandle(std::string profileHandle);
    void SetServerAddress(std::string serverAddress);
    void SetServerPort(int port);
};