//#pragma once

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

#include "Handler.h"

class IClient
{
public:
    virtual void Connect() = 0; 
    virtual int GetHost() = 0;
    virtual int GetPort() = 0;
    virtual void LoadData() = 0;
    virtual char* GetData() = 0;
    virtual void SendData(const char* buf) = 0;
    virtual bool CloseClient(const char* buffer) = 0;
    virtual ~IClient(){};
};
