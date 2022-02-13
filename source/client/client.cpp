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

uint64_t Client::lastSentSeqn;

Client::Client(std::string profileHandle, std::string serverAddress, int serverPort)
    : profileHandle(profileHandle), serverAddress(serverAddress), serverPort(serverPort)
{
    lastSentSeqn = 0;
}

Client::Client() { lastSentSeqn = 0; }

void Client::Listen()
{
    char               buffer[BUFFER_SIZE];
    struct sockaddr_in servAddr;

    Message::Packet p;
    memset(&p, 0, sizeof(p));

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(this->serverPort);
    servAddr.sin_addr.s_addr = inet_addr(this->serverAddress.c_str());

    while (isListening)
    {
        int n, len;

        n = recvfrom(this->socketDescr, &p, sizeof(p), MSG_WAITALL, NULL, NULL);

        if (n > 0)
        {
            printf("Server replied:\n");
            printf("type: %s\n", Message::TypeToStr(p.type));

            switch (p.type)
            {
            case PACKET_ACCEPT_CONN_CMD:
                connected = true;
                std::cout << "Client is connected" << std::endl;
                break;
            case PACKET_REJECT_CONN_CMD:
                std::cout << "Connection was rejected (too many clients connected)" << std::endl;
                Shutdown();
                break;
            case PACKET_NOTIFICATION:
                std::cout << "==============" << std::endl;
                std::cout << p.payload << std::endl;
                std::cout << "==============" << std::endl;
                break;
            default:
                std::cerr << "Client should not receive this message" << std::endl;
                Shutdown();
                break;
            }
        }
    }

    std::cout << "Terminando thread de recebimento de mensagens" << std::endl;
}

int Client::SendMessageToServer(Message::Packet message)
{
    struct sockaddr_in server_addr;
    char               buffer[bufferSize];

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(this->serverPort);
    server_addr.sin_addr.s_addr = inet_addr(this->serverAddress.c_str());

    int r = sendto(this->socketDescr, &message, sizeof(message), 0, (struct sockaddr*)&server_addr,
                   sizeof(server_addr));

    // printf("Data Sent: %s [return value: %d]\n", buffer, r);

    return r;
}

void Client::Shutdown()
{
    this->SendMessageToServer(Message::MakeDisconnectCommand(lastSentSeqn, this->profileHandle));

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

        int r = this->SendMessageToServer(
            Message::MakeConnectCommand(lastSentSeqn, this->profileHandle));
        // printf("%d\n", r);

        this->listeningThread = std::make_unique<std::thread>(&Client::Listen, this);
    }

    return 0;
}

int Client::Connect(std::string profileHandle, std::string serverAddress, int serverPort)
{
    this->profileHandle = profileHandle;
    this->serverAddress = serverAddress;
    this->serverPort    = serverPort;

    this->Connect();
    return 0;
}

int Client::FollowUser(std::string profile)
{
    std::cout << "Seguindo usuário " << profile << std::endl;

    this->SendMessageToServer(Message::MakeFollowCommand(lastSentSeqn, profile));

    return 0;
}

int Client::Post(std::string message)
{
    if (message.length() <= 128)
    {
        std::cout << "Enviando mensagem" << std::endl;
        this->SendMessageToServer(Message::MakeSendCommand(lastSentSeqn, message));
        return 0;
    }
    else
    {
        std::cerr << "Mensagem maior que o máxmo permitido (128 caracteres)" << std::endl;
        return 1;
    }
}

void Client::SetProfileHandle(std::string profileHandle) { this->profileHandle = profileHandle; }

void Client::SetServerAddress(std::string serverAddress) { this->serverAddress = serverAddress; }

void Client::SetServerPort(int port) { this->serverPort = port; }