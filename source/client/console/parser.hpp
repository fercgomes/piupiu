#pragma once
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

struct CommandParserResult
{
    bool                     foundMatch;
    std::string              command;
    std::vector<std::string> arguments;

    CommandParserResult() : foundMatch(false) {}

    CommandParserResult(std::string command, std::vector<std::string> arguments) : foundMatch(true)
    {
        this->command   = command;
        this->arguments = arguments;
    }
};

CommandParserResult ParseInput(std::string input);