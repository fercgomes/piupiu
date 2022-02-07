#include "client.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#define BUFFER_SIZE 1024

Client::Client(std::string profileHandle, std::string serverAddress, int serverPort)
    : profileHandle(profileHandle), serverAddress(serverAddress), serverPort(serverPort)
{
}

void Client::Listen()
{
    char               buffer[BUFFER_SIZE];
    struct sockaddr_in servAddr;

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(this->serverPort);
    servAddr.sin_addr.s_addr = inet_addr(this->serverAddress.c_str());

    while (isListening)
    {
        int n, len;

        n = recvfrom(this->socketDescr, (char*)buffer, BUFFER_SIZE, MSG_WAITALL, NULL, NULL);

        if (n > 0)
        {
            buffer[n] = '\0';
            printf("Server: %s\n", buffer);
        }
    }
}

void Client::Shutdown() { std::cout << "O programa foi terminado." << std::endl; }

int Client::Connect()
{
    std::cout << "Conectando no servidor " << serverAddress << ":" << serverPort << " como "
              << profileHandle << std::endl;

    if (this->socketDescr = socket(PF_INET, SOCK_DGRAM, 0) < 0)
    { std::cerr << "Não foi possível abrir o socket." << std::endl; }
    else
    {
        std::cout << "Socket aberto." << std::endl;
        this->listeningThread = std::make_unique<std::thread>(&Client::Listen, this);
    }

    return 0;
}

int Client::FollowUser(std::string profile)
{
    std::cout << "Seguindo usuário " << profile << std::endl;

    char               buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    std::stringstream  sstream;

    sstream << "FOLLOW " << profile;
    std::string message = sstream.str();
    strcpy(buffer, message.c_str());
    std::cout << buffer << std::endl;
    std::cout << this->socketDescr << std::endl;
    std::cout << this->serverAddress << std::endl;
    std::cout << this->serverPort << std::endl;

    memset(&servaddr, '\0', sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(this->serverPort);
    servaddr.sin_addr.s_addr = inet_addr(this->serverAddress.c_str());

    int r = sendto(this->socketDescr, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&servaddr,
                   sizeof(servaddr));

    std::cout << "Sent: " << r << std::endl;

    return 0;
}

int Client::SendMessage(std::string recipient, std::string message)
{
    if (message.length() <= 128)
    {
        std::cout << "Enviando mensagem para " << recipient << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Mensagem maior que o máxmo permitido (128 caracteres)" << std::endl;
        return 1;
    }
}