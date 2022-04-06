#pragma once
#include "parser.hpp"
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

static std::vector<std::string> split(std::string const& input)
{
    std::istringstream       buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer), std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}

template<typename T>
static std::vector<T> slice(std::vector<T>& arr, int X, int Y)
{
    // Starting and Ending iterators
    auto start = arr.begin() + X;
    auto end   = arr.begin() + Y + 1;

    // To store the sliced vector
    std::vector<T> result(Y - X + 1);

    // Copy vector using copy function()
    copy(start, end, result.begin());

    // Return the final sliced vector
    return result;
}

static bool ParseCommand(std::string s, std::string& out)
{
    if (s.size() > 1 && s[0] == '/')
    {
        std::string clean = s.substr(1);
        out.swap(clean);
        return true;
    }
    else
    {
        return false;
    }
}

CommandParserResult ParseInput(std::string input)
{
    std::string command;

    if (input.size() > 0)
    {
        auto pieces = split(input);

        if (pieces.size() > 0)
        {
            if (ParseCommand(pieces[0], command))
            {
                std::cout << command << std::endl;
                return CommandParserResult(command, slice(pieces, 1, pieces.size() - 1));
            }
            else
            {
                return CommandParserResult();
            }
        }
        else
        {

            return CommandParserResult();
        }
    }
    else
    {
        return CommandParserResult();
    }
}