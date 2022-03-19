#include <iostream>
#include "server.hpp"

int main()
{
    Server* server = new Server("0.0.0.0", 5000);

    server->Start();

    return 0;
}