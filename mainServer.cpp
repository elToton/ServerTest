#include <iostream>
#include <regex>
#include "Server.h"

int main(int argc, char *argv[])
{
    Handler* handler = new Handler();

    regex ip("^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9]).){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$");
    


    if (argc != 4)
    { 
        std::cout << "Please, enter all the parametres in order: TCP port, UDP port, IP address.\n";
        return 0;
    }

    uint16_t TCP_Port = atoi(argv[1]);
    uint16_t UDP_Port = atoi(argv[2]); 
    const char* server_address = argv[3];

    if ((!regex_match(server_address, ip)) or (TCP_Port < 1024 or TCP_Port > 49151) or ((UDP_Port < 1024 or UDP_Port > 49151)))
    {
        std::cout << "Please, enter all the parametres in order: TCP port, UDP port, IP address.\n";
        return 0;
    }

    Server* server = new Server(handler, TCP_Port, UDP_Port, server_address);

    server->Connect();

    while(1)
    {
        server->SetSelect();

        server->ClientsContact();
    }
}