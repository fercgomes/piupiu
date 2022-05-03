#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <ncurses.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "client.hpp"
#include "confirmation-buffer.hpp"
#include "console/console.hpp"
#include "console/parser.hpp"

#ifdef USE_GUI
#include "gui/gui.hpp"
#endif

sig_atomic_t signaled = 0;

enum ClientStatus
{
    NotConnected = 0,
    Connected    = 1
};

void HelpUsage()
{
    std::cout << "Uso correto:" << std::endl;
    std::cout << "./Client usuario server_ip server_port" << std::endl;
}

int main(int argc, const char** argv)
{
    if (argc == 4)
    {
        std::string              profile(argv[1]);
        std::string              serverAddress(argv[2]);
        int                      serverPort(atoi(argv[3]));
        bool                     shouldExit = false;
        std::string              commandInput;
        std::vector<std::string> screenBuffer;

        enum ClientStatus status = ClientStatus::NotConnected;

        Client*           client  = new Client(profile, serverAddress, serverPort);
        ConsoleInterface* console = new ConsoleInterface(client);

        client->Connect();

        console->Start();

        signal(SIGINT, [](int signum) { signaled = 1; });

        client->Shutdown();

        endwin();

        return 0;
    }
    else
    {
#ifdef USE_GUI
        Client* client = new Client();
        GUI*    gui    = new GUI(client);
        return gui->run();
#else
        // std::cout << el << std::endl;

#endif
    }
}
