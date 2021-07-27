#include "TCP_Client.h"

class UDP_Client : public IClient
{
private:
    Handler* handler;
    int sock;
    struct sockaddr_in server_address;
    socklen_t sockSize;
    uint16_t port;
    const char* IP;
    char* buf = new char[1024];
public:
    UDP_Client(Handler* handler, uint16_t port, const char* address);
    ~UDP_Client();
    void Connect();
    int GetHost();
    int GetPort();
    void LoadData();
    char* GetData();
    void SendData(const char* buffer);
    bool CloseClient(const char* buffer);
};
