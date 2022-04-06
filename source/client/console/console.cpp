#include "console.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <regex>
#include "parser.hpp"

#define ctrl(x) ((x)&0x1f)

void DrawTitle()
{
    std::string title = "PiuPiu";
    const int   size  = title.size();

    const int xPos = (COLS / 2) - size;
    const int yPos = 1;

    mvaddstr(yPos, xPos, title.c_str());
}

void DrawFrame()
{
    for (int i = 1; i < COLS - 1; i++)
    {
        mvaddch(LINES - 3, i, '-');
    }

    mvaddstr(LINES - 2, 1, "ENVIAR:");
    // mvaddch(LINES - 2, 9, '|');
}

void ConsoleInterface::IncomingMessageHandler(std::string message, enum MessageType type)
{
    auto timestamp = std::chrono::system_clock::now();

    switch (type)
    {
    case Notification:
        messages.push_back(MakeNotificationMessage(message, 0));
        break;
    case Info:
        messages.push_back(MakeInfoMessage(message, 0));
        break;
    case Error:
        messages.push_back(MakeErrorMessage(message, 0));
        break;
    }
}

ConsoleInterface::ConsoleInterface(Client* client) : client(client)
{
    running = false;

    client->SetMessageHandlerFunc([this](std::string message, enum MessageType type) {
        this->IncomingMessageHandler(message, type);
    });
}

void ConsoleInterface::Init()
{
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
}

void ConsoleInterface::HandleUserInput(std::string input)
{
    // Commands: /follow, /help, /exit

    /** Follow Command */
    auto parsedCommand = ParseInput(input);

    if (parsedCommand.foundMatch)
    {
        std::string              command   = parsedCommand.command;
        std::vector<std::string> arguments = parsedCommand.arguments;

        mvaddstr(3, 3, command.c_str());

        if (!command.compare("follow"))
        {
            if (arguments.size() == 1)
            {
                std::string username = arguments[0];
                client->FollowUser(username);
                //
            }
            else
            {
                messages.push_back(MakeErrorMessage("Um nome de usuário precisa ser informado", 0));
                // Wrong arguments
            }

            return;
        }

        if (!command.compare("help"))
        {
            //
            messages.push_back(
                MakeInfoMessage(std::string("teste teste\nteste teste\nteste teste"), 0));
            return;
        }

        if (!command.compare("me"))
        {
            //
            return;
        }

        if (!command.compare("exit"))
        {
            //
            Stop();
            return;
        }
    }
    else
    {
        // send message
        client->Post(userInput);
    }
}

void ConsoleInterface::GetUserInput(int ch)
{
    switch (ch)
    {
    case KEY_BACKSPACE:
    case 127:
        if (userInput.size() > 0) { userInput.erase(userInput.end() - 1); }
        break;
    case KEY_ENTER:
    case '\n':
    case '\r':
        HandleUserInput(userInput);
        userInput.clear();
        break;
    case ctrl('c'):
        running = false;
        break;
    default:
        userInput += ch;
        break;
    }
}

void ConsoleInterface::ClearUserInput() { userInput.erase(); }

void ConsoleInterface::DrawUserInput()
{
    const int inputStartX = LINES - 2;
    const int inputStartY = 9;

    mvaddstr(inputStartX, inputStartY, userInput.c_str());
}

void ConsoleInterface::DrawMessages()
{
    if (messages.size() > 0)
    {
        auto message = messages[messages.size() - 1];

        mvaddstr(3, 2, message.body.c_str());
    }
}

void ConsoleInterface::DrawStatus()
{
    const int x = 4, y = 0;
    mvaddstr(y, x, "Status:");

    if (client->IsConnected())
    {
        attron(COLOR_PAIR(2));
        mvaddstr(y, x + 7, " Conectado em ");
        mvaddstr(y, x + 7 + 14, client->GetHostStr().c_str());
        attroff(COLOR_PAIR(2));
    }
    else
    {
        attron(COLOR_PAIR(1));
        mvaddstr(y, x + 7, " Não conectado");
        attroff(COLOR_PAIR(1));
    }
}

void ConsoleInterface::MainLoop()
{
    int ch;

    /** Main drawing loop */
    while (running)
    {
        /** Erase the current screen */
        clear();

        /** Draw frame */
        border('|', '|', '-', '-', '+', '+', '+', '+');
        DrawFrame();
        DrawUserInput();
        DrawTitle();
        DrawMessages();
        DrawStatus();
        move(0, 0);

        /** Wait user input */
        if (client->IsConnected())
        {
            ch = getch();
            GetUserInput(ch);
        }
        else
        {
        }

        /** Update the current screen */
        refresh();
    }

    /** NCurses cleanup */
    endwin();
}

int ConsoleInterface::Start()
{
    Init();
    running = true;

    /** Launch main listening thread: blocking here */
    MainLoop();
}

int ConsoleInterface::Stop() { running = false; }