#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

class Server
{
private:
    std::string bindAddress;
    int         bindPort;

    int                socketDescr;
    struct sockaddr_in socketAddress;
    const int          bufferSize = 1024;

    bool                         isListening     = true;
    std::unique_ptr<std::thread> listeningThread = nullptr;

    void Listen();

    void ParseInput(const char* buffer);

public:
    Server(std::string bindAddress, int bindPort);

    /* Inicia o servidor */
    void Start();

    /* Finaliza o servidor */
    void Stop();
};