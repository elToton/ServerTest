#include "TCP_Client.h"

TCP_Client::TCP_Client(Handler* handler, uint16_t port, const char* address) 
    : handler(handler), port(port), IP(address){}

void TCP_Client::Connect()
{
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    if (connect(sock, reinterpret_cast<const struct sockaddr*>(&server_address), sockSize) < 0)
    {
        perror("connection error\n");
        close(sock);
        exit(3);
    }
}

int TCP_Client::GetHost()
{
    return server_address.sin_addr.s_addr;
}

int TCP_Client::GetPort()
{
    return server_address.sin_port;
}

void TCP_Client::LoadData()
{
    unsigned int package_size;
    memset(buf, 0, 1024);

    ssize_t read_bytes = recv(sock, (char*)&package_size, sizeof(package_size), 0);
    if (read_bytes == 0)
    {
        perror("Server crashed, please try to reconnect.\n");
        close(sock);
        exit(0);
    } else if (read_bytes < 0)
    {
        perror("recv error\n");
        close(sock);
        exit(1);
    }
    read_bytes = recv(sock, buf, package_size, 0);
    if (read_bytes == 0)
    {
        perror("Server crashed, please try to reconnect.\n");
        close(sock);
        exit(0);
    } else if (read_bytes < 0)
    {
        perror("recv error\n");
        close(sock);
        exit(1);
    }
}

char* TCP_Client::GetData()
{
    return buf;
}

void TCP_Client::SendData(const char* buffer)
{
    unsigned int write_bytes;
    if (send(sock, (char*)&write_bytes, sizeof(write_bytes), 0) < 0)
    {
        perror("sendto error\n");
        close(sock);
        exit(1);
    }
    if (send(sock, buffer, strlen(buffer), 0) < 0)
    {
        perror("sendto error\n");
        close(sock);
        exit(1);
    }
}

bool TCP_Client::CloseClient(const char* buffer)
{
    if (handler->CloseConnection(buffer))
        return true;
    else    
        return false;
}

TCP_Client::~TCP_Client()
{
    delete [] buf;
    shutdown(sock, 0);
    close(sock);
}
