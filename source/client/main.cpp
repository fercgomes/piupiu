#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.hpp"

#define PORT 5000

int test()
{
    const int          port   = 5000;
    const char*        ipaddr = "127.0.0.1";
    int                sockfd;
    struct sockaddr_in server_addr;
    char               buffer[1024];
    socklen_t          addr_size;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ipaddr);

    strcpy(buffer, "Hello Server\n");
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[+]Data Sent: %s\n", buffer);
    return 0;
}

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

        while (!shouldExit)
        {
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
                exit(EXIT_FAILURE);
            }
        }

        return 0;
    }
    else
    {
        std::cerr << "Número de argumentos inválido." << std::endl;
        return -1;
    }
}
