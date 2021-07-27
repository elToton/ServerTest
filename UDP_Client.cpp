#include "UDP_Client.h"

UDP_Client::UDP_Client(Handler* handler, uint16_t port, const char* address) 
    : handler(handler), port(port), IP(address){}

void UDP_Client::Connect()
{
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        perror("establishing socket error.");
        exit(1);
    }

    memset((char*)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, IP, &server_address.sin_addr) == 0)
    {
        perror("inet_pton failed\n");
        close(sock);
        exit(2);
    }

    sockSize = sizeof(server_address);
    std::cout << "\nClient Socket created.";
}

int UDP_Client::GetHost()
{
    return server_address.sin_addr.s_addr;
}

int UDP_Client::GetPort()
{
    return server_address.sin_port;
}

void UDP_Client::LoadData()
{
    ssize_t read_bytes = recvfrom(sock, buf, sizeof(buf), 0, 
        reinterpret_cast<struct sockaddr*>(&server_address), &sockSize);
    if(read_bytes < 0)
    {
        perror("recvfrom error\n");
        close(sock);
        exit(3);
    }
}

char* UDP_Client::GetData()
{
    return buf;
}

void UDP_Client::SendData(const char* buffer)
{
    if (sendto(sock, buffer, sizeof(buffer), 0, 
        reinterpret_cast<struct sockaddr*>(&server_address), sockSize) == -1)
        {
            perror("sendto error\n");
            close(sock);
            exit(4);
        }
}

bool UDP_Client::CloseClient(const char* buffer)
{
    if (handler->CloseConnection(buffer))
        return true;
    else
        return false;
}

UDP_Client::~UDP_Client()
{
    close(sock);
}