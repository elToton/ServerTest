//#pragma once
#include "IClient.h"

class TCP_Client : public IClient
{
private:
    Handler* handler;
    char* buf = new char[1024];
    int sock;
    struct sockaddr_in server_address;
    socklen_t sockSize;
    const char* IP;
    uint16_t port;
public:
    TCP_Client(Handler* handler, uint16_t port, const char* address);
    ~TCP_Client();
    void Connect();
    int GetHost();
    int GetPort();
    void LoadData();
    char* GetData();
    void SendData(const char* buffer);
    bool CloseClient(const char* buffer);

    int sendall(int s, char *buf, int len, int flags);
    int recvall(int s, char *buf, int len, int flags);
};