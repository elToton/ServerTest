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

char* handler(char* buf)
{
    string str, t;
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

int main()
{
    int t_listener;
    int u_listener;
    struct sockaddr_in udp_server_address;
    struct sockaddr_in tcp_server_address;
    char buf[1024];
    int bytes_read;
    int recv_len;

    u_listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    t_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((u_listener < 0) and (t_listener < 0))
    {
        perror("establishing socket error.");
        exit(1);
    }

    cout << "SERVER: Socket for server was successfully created\n";

    fcntl(t_listener, F_SETFL, O_NONBLOCK);
    fcntl(u_listener, F_SETFL, O_NONBLOCK);

    memset((char*)&tcp_server_address, 0, sizeof(tcp_server_address));
    tcp_server_address.sin_family = AF_INET;
    tcp_server_address.sin_port = htons(TCP_SERVER_PORT);
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;

    memset((char*)&udp_server_address, 0, sizeof(udp_server_address));
    udp_server_address.sin_family = AF_INET;
    udp_server_address.sin_port = htons(UDP_SERVER_PORT);
    udp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t tcp_size = sizeof(tcp_server_address);
    socklen_t udp_size = sizeof(udp_server_address);

    int t_ret = bind(t_listener, reinterpret_cast<struct sockaddr*>(&tcp_server_address), tcp_size);
    int u_ret = bind(u_listener, reinterpret_cast<struct sockaddr*>(&udp_server_address), udp_size);

    if ((u_ret < 0) or (t_ret < 0))
    {
        perror("binding connection. Socket has already been establishing.\n");
        exit(2);
    }

    listen(t_listener, 2);

    vector<pair<int, bool> > clients;
    clients.clear();


    int nfds = getdtablesize();

    bool isExit = false;

    fd_set readset;
    fd_set activeset;
    FD_ZERO(&readset);
    FD_ZERO(&activeset);
    FD_SET(t_listener, &activeset);
    FD_SET(u_listener, &activeset);

    while (1)
    {
        memcpy(&readset, &activeset, sizeof(activeset));

        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (select(nfds, &readset, (fd_set*)0, (fd_set*)0, &timeout) <= 0)
        {
            cout << "\nThere are no clients in the select queue. Successful exit.\n";
            exit(0);
        }

        if (FD_ISSET(t_listener, &readset))
        {
            int sock = accept(t_listener,
            reinterpret_cast<struct sockaddr*>(&tcp_server_address), &tcp_size);
            if (sock < 0)
            {
                perror("Can't accepting client.\n");
                exit(3);
            }
            FD_SET(sock, &activeset);

            fcntl(sock, F_SETFL, O_NONBLOCK);

            clients.push_back(make_pair(sock, 1));
        }

        if (FD_ISSET(u_listener, &readset))
        {
            fcntl(u_listener, F_SETFL, O_NONBLOCK);

            clients.push_back(make_pair(u_listener, 0));
        }

        sort(clients.begin(), clients.end());

        for (size_t i = 0; i < clients.size(); ++i)
        {
            if (FD_ISSET(clients[i].first, &readset))
            {
                if (clients[i].second){
                    recv(clients[i].first, buf, 1024, 0);
                }
                else{
                     if (recvfrom(clients[i].first, buf, sizeof(buf), 0, 
                    reinterpret_cast<struct sockaddr*>(&udp_server_address), &tcp_size) == -1){
                        perror("sendto recv");
                        exit(4);
                    }
                }
            
                if (is_client_connection_close(buf))
                {
                    FD_CLR(clients[i].first, &activeset);
                    if (clients[i].second)
                        close(clients[i].first);

                    clients.erase(clients.begin() + i);
                    
                    continue;
                }

                if (clients[i].second)
                    send(clients[i].first, handler(buf), 1024, 0);
                else{
                    if (sendto(clients[i].first, handler(buf), strlen(buf), 0, 
                    reinterpret_cast<struct sockaddr*>(&udp_server_address), tcp_size) == -1){
                        perror("sendto recv");
                        exit(5);
                    }
                }

                if (!(clients[i].second)) clients.erase(clients.begin() + i);
            }
        }
    }
    close(u_listener);
    return 0;
}
