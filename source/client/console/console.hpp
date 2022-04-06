#pragma once
#include <memory>
#include <ncurses.h>
#include <thread>
#include "../client.hpp"
#include "message.hpp"

class ConsoleInterface
{
private:
    void Init();
    void MainLoop();
    void HandleUserInput(std::string input);

    void GetUserInput(int ch);
    void ClearUserInput();
    void DrawUserInput();
    void DrawMessages();
    void DrawStatus();
    void IncomingMessageHandler(std::string message, enum MessageType type);

    Client*                      client;
    std::string                  userInput;
    bool                         running;
    std::unique_ptr<std::thread> mainLoopThread;
    std::vector<ConsoleMessage>  messages;

public:
    ConsoleInterface(Client* client);

    int Start();
    int Stop();
};