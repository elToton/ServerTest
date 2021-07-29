#include <iostream>
#include <regex>
#include "UDP_Client.h"

using namespace std;

int main(int argc, char *argv[])
{
    Handler *handler = new Handler();

    IClient* client = nullptr;

    regex ip("^(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9]).){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$");

    if (argc != 4)
    { 
        std::cout << "Please, enter all the parametres in order: TCP port, UDP port, IP address.\n";
        return 0;
    }

    uint16_t port = atoi(argv[2]);
    const char* server_address = argv[3];

    if ((!regex_match(server_address, ip)) or (port < 1024 or port > 49151))
    {
        std::cout << "Please, enter all the parametres in order: TCP port, UDP port, IP address.\n";
        return 0;
    }

    if (strcmp(argv[1], "UDP") == 0)
        client = new UDP_Client(handler, port, server_address);
    else
        client = new TCP_Client(handler, port, server_address);

    client->Connect();

    char *str = new char[1024];
    cout << "Waiting for server confirmation...\n";

    cout << "Connection established. "
         << "Enter # for close the conection\n";

    while (1)
    {
        cin.getline(str, 1024);
        char *buf = new char[strlen(str) + 1];
        strcpy(buf, str);

        client->SendData(buf);

        if (client->CloseClient(str))
            break;

        client->LoadData();

        cout << client->GetData() << endl;
        delete[] buf;
    }
    delete[] str;
}