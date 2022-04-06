#pragma once
#include <memory>
#include <mutex>
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
    void InputLoop();

    void GetUserInput(int ch);
    void ClearUserInput();
    void DrawUserInput();
    void DrawMessages();
    void DrawStatus();
    void IncomingMessageHandler(std::string message, enum MessageType type);

    Client*                      client;
    std::string                  userInput;
    bool                         running;
    bool                         shouldRefresh;
    std::unique_ptr<std::thread> mainLoopThread;
    std::unique_ptr<std::thread> inputThread;
    std::vector<ConsoleMessage>  messages;
    std::mutex                   mutex;
    std::mutex                   userInputMutex;
    int                          messageCursor = 0;

public:
    ConsoleInterface(Client* client);

    int Start();
    int Stop();
};