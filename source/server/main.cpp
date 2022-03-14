#include <iostream>
#include "server.hpp"

int main()
{
    Server* server = new Server("127.0.0.1", 5000);

    server->Start();

    return 0;
}