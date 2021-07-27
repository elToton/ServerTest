#include "Handler.h"

Handler::Handler() : symbol("#"){}

Handler::Handler(std::string symb) : symbol(symb){}

char* Handler::Handle(char* buf)
{
    std::string str, t;
    str = buf;
    int sum = 0;
    int k = 0;
    bool digit = false;

    for(int i = 0; i <= str.size(); ++i)
    {
        if (isdigit(str[i])){
            t += str[i];
            digit = true;
        }
        else {
            sum += atoi(t.c_str());
            t = "";
            if (digit){
                ++k;
                digit = false;
            }
        }
    }

    if (k != 0)
        sprintf(buf, "%d", sum);

    return buf;
}

bool Handler::CloseConnection(const char* buf)
{
    if (strcmp(buf, symbol.c_str()) == 0)
        return true;
        
    return false;
}