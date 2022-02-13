#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "client.hpp"

#ifdef USE_GUI
#include "gui/gui.hpp"
#endif

sig_atomic_t signaled = 0;

std::array<std::string, 2> ParseInput(std::string input)
{
    int delimiterIndex = -1;
    int i              = 0;
    while (i < input.length() && delimiterIndex == -1)
    {
        if (isspace(input[i])) { delimiterIndex = i; }
        else
        {
            i++;
        }
    }

    if (delimiterIndex != -1)
    {
        std::array<std::string, 2> arr;
        std::string                command  = input.substr(0, delimiterIndex);
        std::string                argument = input.substr(delimiterIndex + 1, input.length() - 1);
        arr[0]                              = command;
        arr[1]                              = argument;
        return arr;
    }
    else
    {
        throw std::invalid_argument("Entrada mal formada.");
    }
}

void CommandHandler(Client* client, std::string command, std::string arguments) {}

void ListeningHandler(Client* client)
{
    bool        shouldExit = false;
    std::string commandInput;

    while (!shouldExit)
    {

        // Mesmo sinalizando, o processo fica bloqueado aqui, e só sai quando ler a entrada.
        // Uma possibilidade é colocar isso em uma thread, e escutar o SIGINT aqui, e quando
        // pegar um SIGINT, matar a thread.
        std::getline(std::cin, commandInput);

        try
        {
            auto parsedInput = ParseInput(commandInput);
            std::cout << parsedInput[0] << std::endl;
            std::cout << parsedInput[1] << std::endl;

            if (parsedInput[0].compare("FOLLOW") == 0) { client->FollowUser(parsedInput[1]); }
        }
        catch (const std::invalid_argument& e)
        {
            std::cerr << e.what() << std::endl;
            shouldExit = true;
        }
    }
}

int main(int argc, const char** argv)
{
    if (argc == 4)
    {
        std::string profile(argv[1]);
        std::string serverAddress(argv[2]);
        int         serverPort(atoi(argv[3]));
        bool        shouldExit = false;
        std::string commandInput;

        Client* client = new Client(profile, serverAddress, serverPort);
        client->Connect();

        signal(SIGINT, [](int signum) { signaled = 1; });

        std::thread inputThread(ListeningHandler, client);

        while (!signaled)
            ;

        inputThread.join();

        client->Shutdown();

        // while (!shouldExit)
        // {
        //     if (signaled == 0)
        //     {
        //         // Mesmo sinalizando, o processo fica bloqueado aqui, e só sai quando ler a
        //         entrada.
        //         // Uma possibilidade é colocar isso em uma thread, e escutar o SIGINT aqui, e
        //         quando
        //         // pegar um SIGINT, matar a thread.
        //         std::getline(std::cin, commandInput);

        //         try
        //         {
        //             auto parsedInput = ParseInput(commandInput);
        //             std::cout << parsedInput[0] << std::endl;
        //             std::cout << parsedInput[1] << std::endl;

        //             if (parsedInput[0].compare("FOLLOW") == 0)
        //             { client->FollowUser(parsedInput[1]); }
        //         }
        //         catch (const std::invalid_argument& e)
        //         {
        //             std::cerr << e.what() << std::endl;
        //             exit(EXIT_FAILURE);
        //         }
        //     }
        //     else
        //     {
        //         client->Shutdown();
        //         shouldExit = true;
        //     }
        // }

        return 0;
    }
    else
    {
#ifdef USE_GUI
        Client* client = new Client();
        GUI*    gui    = new GUI(client);
        return gui->run();
#else
        std::cout << "Argumentos inválidos." << std::endl;
#endif
    }
}
