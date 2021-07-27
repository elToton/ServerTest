#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <utility>

#include "Handler.h"

using namespace std;

class Server
{
private:
    Handler* handler;
    uint16_t portTCP;
    uint16_t portUDP;
    const char *IP;
    char* buf = new char[1024];
    int t_listener;
    int u_listener;
    struct sockaddr_in udp_server_address;
    struct sockaddr_in tcp_server_address;
    socklen_t tcp_size;
    socklen_t udp_size;
    int bytes_read;
    int recv_len;
    vector<pair<int, bool>> clients;
    int nfds = getdtablesize();
    fd_set readset;
    fd_set activeset;
public:
    Server(Handler* handler, uint16_t portTCP, uint16_t portUDP, const char *address);
    void Connect();
    void SetSelect();
    void LoadData(size_t i);
    void SendData(size_t i, const char* buffer);
    void ClientsContact();
    ~Server();
};