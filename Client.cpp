#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>

#define SERVER_ADRESS "127.0.0.1"
#define UDP_SERVER_PORT 1641
#define TCP_SERVER_PORT 1956

using namespace std;

bool is_client_connection_close(const char* msg)
{
    if (strcmp(msg, "#") == 0)
        return true;

    return false;
}

int main(int argc, const char* argv[])
{
    int sock;

    struct sockaddr_in server_address;

    bool isTCP = true;

    if (strcmp(argv[1], "UDP") == 0) 
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        isTCP = false;
    } else
    {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    if (sock < 0)
    {
        perror("establishing socket error.");
        exit(1);
    }

    memset((char*)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    if (isTCP) server_address.sin_port = htons(TCP_SERVER_PORT);
    else server_address.sin_port = htons(UDP_SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADRESS, &server_address.sin_addr) == 0)
    {
        perror("inet_pton failed\n");
        exit(1);
    }

    socklen_t size = sizeof(server_address);
    cout << "\nClient Socket created.";

    if (isTCP)
    {
        int ret = connect(sock, 
        reinterpret_cast<const struct sockaddr*>(&server_address), size);
    }

    char buf[1024];
    cout << "Waiting for server confirmation...\n";

    cout << "Connection established. " << "Enter # for close the conection\n";

    while(1){
        cin.getline(buf, 1024);
        if (isTCP)
            send(sock, buf, 1024, 0);
        else
            if (sendto(sock, buf, 1024, 0, 
            reinterpret_cast<struct sockaddr*>(&server_address), size) == -1)
            {
                perror("sendto error\n");
                exit(1);
            }

        if (is_client_connection_close(buf))
            break;

        if (isTCP) 
            recv(sock, buf, 1024, 0);
        else
            if (recvfrom(sock, buf, 1024, 0, 
            reinterpret_cast<struct sockaddr*>(&server_address), &size) == -1)
            {
                perror("recvfrom error\n");
                exit(1);
            }

        cout << buf << endl;
    }

    close(sock);
}