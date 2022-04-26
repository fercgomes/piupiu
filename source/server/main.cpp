#include <iostream>
#include "server.hpp"

int main()
{
    const char* bindIpVar   = std::getenv("BIND_IP");
    const char* bindPortVar = std::getenv("BIND_PORT");
    const char* peersList   = std::getenv("PEERS");
    const char* primaryVar  = std::getenv("PRIMARY");

    std::string bindIp(bindIpVar ? bindIpVar : "");
    int         bindPort = bindPortVar ? atoi(bindPortVar) : -1;
    std::string peers(peersList ? peersList : "");
    bool        primary = primaryVar ? strcmp(primaryVar, "true") == 0 : false;

    std::cout << bindIp << ", " << bindPort << ", " << peersList << ", " << primary << std::endl;

    Server* server = new Server(bindIp, bindPort, peers, primary);
    server->Start();

    return 0;
}