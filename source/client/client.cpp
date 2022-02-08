#include "client.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

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

    std::cout << "Terminando thread de recebimento de mensagens" << std::endl;
}

void Client::SendMessageToServer(std::string message)
{
    struct sockaddr_in server_addr;
    char               buffer[bufferSize];

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(this->serverPort);
    server_addr.sin_addr.s_addr = inet_addr(this->serverAddress.c_str());

    strcpy(buffer, message.c_str());
    int r = sendto(this->socketDescr, buffer, bufferSize, 0, (struct sockaddr*)&server_addr,
                   sizeof(server_addr));

    printf("Data Sent: %s [return value: %d]\n", buffer, r);
}

void Client::Shutdown()
{
    close(this->socketDescr);
    this->isListening = false;

    std::cout << "O programa foi terminado." << std::endl;
}

int Client::Connect()
{
    std::cout << "Conectando no servidor " << serverAddress << ":" << serverPort << " como "
              << profileHandle << std::endl;

    if ((this->socketDescr = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    { std::cerr << "Não foi possível abrir o socket." << std::endl; }
    else
    {
        std::cout << "Socket aberto (" << this->socketDescr << ")" << std::endl;
        this->listeningThread = std::make_unique<std::thread>(&Client::Listen, this);
    }

    return 0;
}

int Client::FollowUser(std::string profile)
{
    std::cout << "Seguindo usuário " << profile << std::endl;

    this->SendMessageToServer("FOLLOW blah");

    return 0;
}

int Client::Post(std::string recipient, std::string message)
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