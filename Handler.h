//#pragma once

#include <cstring>
#include <string>

class Handler
{
private:
    std::string symbol;
public:
    Handler();
    Handler(std::string symb);
    char* Handle(char* buf);
    bool CloseConnection(const char* buf);
};
