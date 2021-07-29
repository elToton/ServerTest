#include "Server.h"

Server::Server(Handler *handler, uint16_t portTCP, uint16_t portUDP, const char *address)
    : handler(handler), portTCP(portTCP), portUDP(portUDP), IP(address) {}

void Server::Connect()
{
    u_listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    t_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((u_listener < 0) and (t_listener < 0))
    {
        perror("establishing socket error.");
        exit(1);
    }

    int optval = 1;
    setsockopt(t_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(u_listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    cout << "SERVER: Socket for server was successfully created\n";

    fcntl(t_listener, F_SETFL, O_NONBLOCK);
    fcntl(u_listener, F_SETFL, O_NONBLOCK);

    memset((char *)&tcp_server_address, 0, sizeof(tcp_server_address));
    tcp_server_address.sin_family = AF_INET;
    tcp_server_address.sin_port = htons(portTCP);
    tcp_server_address.sin_addr.s_addr = INADDR_ANY;

    memset((char *)&udp_server_address, 0, sizeof(udp_server_address));
    udp_server_address.sin_family = AF_INET;
    udp_server_address.sin_port = htons(portUDP);
    udp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    tcp_size = sizeof(tcp_server_address);
    udp_size = sizeof(udp_server_address);

    int t_ret = bind(t_listener, reinterpret_cast<struct sockaddr *>(&tcp_server_address), tcp_size);
    int u_ret = bind(u_listener, reinterpret_cast<struct sockaddr *>(&udp_server_address), udp_size);

    if ((u_ret < 0) or (t_ret < 0))
    {
        perror("binding connection. Socket has already been establishing.\n");
        exit(2);
    }

    listen(t_listener, 2);

    clients.clear();

    FD_ZERO(&readset);
    FD_ZERO(&activeset);
    FD_SET(t_listener, &activeset);
    FD_SET(u_listener, &activeset);
}

void Server::SetSelect()
{
    memcpy(&readset, &activeset, sizeof(activeset));

    if (select(nfds, &readset, (fd_set *)0, (fd_set *)0, NULL) <= 0)
    {
        cout << "\nThere are no clients in the select queue. Successful exit.\n";
        exit(0);
    }

    if (FD_ISSET(t_listener, &readset))
    {
        int sock = accept(t_listener,
            reinterpret_cast<struct sockaddr *>(&tcp_server_address), &tcp_size);
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
}

void Server::LoadData(size_t i)
{
    unsigned int package_size;
    memset(buf, 0, 1024);
    if (clients[i].second)
    {
        recv(clients[i].first, (char*)&package_size, sizeof(package_size), 0);
        bytes_read = recv(clients[i].first, buf, package_size, 0);
    }
    else
    {
        if (recvfrom(clients[i].first, (char*)&package_size, sizeof(package_size), 0, 
        reinterpret_cast<struct sockaddr*>(&udp_server_address), &udp_size) < 0)
        {
            perror("recvfrom error\n");
            exit(4);
        }
        bytes_read = recvfrom(clients[i].first, buf, package_size, 0,
            reinterpret_cast<struct sockaddr *>(&udp_server_address), &udp_size);
        if (bytes_read == -1)
        {
            perror("sendto recv");
            exit(4);
        }
    }
}

void Server::SendData(size_t i, char *buffer)
{
    unsigned int write_bytes = strlen(buffer);
    if (clients[i].second)
    {
        send(clients[i].first, (char*)&write_bytes, sizeof(write_bytes), 0);
        send(clients[i].first, buffer, strlen(buffer), 0);
    }
    else
    {
        if (sendto(clients[i].first, (char*)&write_bytes, sizeof(write_bytes), 0,
        reinterpret_cast<struct sockaddr*>(&udp_server_address), udp_size) == -1)
        {
            perror("send error\n");
            exit(5);
        }
        if (sendto(clients[i].first, buffer, strlen(buffer), 0,
                   reinterpret_cast<struct sockaddr *>(&udp_server_address), udp_size) == -1)
        {
            perror("sendto recv");
            exit(5);
        }
    }
}

void Server::ClientsContact()
{
    for (size_t i = 0; i < clients.size(); ++i)
    {

        if (FD_ISSET(clients[i].first, &readset))
        {
            LoadData(i);

            if ((bytes_read <= 0) or (handler->CloseConnection(buf)))
            {
                if (clients[i].second)
                {
                    FD_CLR(clients[i].first, &activeset);
                    close(clients[i].first);
                }
                clients.erase(clients.begin() + i);
                continue;
            }

            SendData(i, handler->Handle(buf));

            if (!(clients[i].second))
                clients.erase(clients.begin() + i);
        }
    }
}

Server::~Server()
{
    delete [] buf;
    close(t_listener);
    close(u_listener);
}