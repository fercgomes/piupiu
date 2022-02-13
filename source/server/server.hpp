#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include <Packet.hpp>
#include "SessionManager.hpp"

class Server
{
private:
    std::string bindAddress;
    int         bindPort;

    int                socketDescr;
    struct sockaddr_in socketAddress;
    const int          bufferSize = 2048;

    bool                         isListening     = true;
    std::unique_ptr<std::thread> listeningThread = nullptr;
    std::vector<std::thread>     messageHandlerThreads;

    SessionManager* sessionManager;

    void Listen();
    void ParseInput(const char* buffer);
    void MessageHandler(Message::Packet message);

public:
    Server(std::string bindAddress, int bindPort);

    /* Inicia o servidor */
    void Start();

    /* Finaliza o servidor */
    void Stop();
};