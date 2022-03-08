#pragma once

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <stdio.h>

#define MAX_MESSAGE_SIZE 256

class Socket{
    private:
        int sockfd;
        struct sockaddr_in addr;
        struct sockaddr_in oth_addr;
        socklen_t clilen;
        bool connected;
        
    public:
        Socket(int port = 0);
        ~Socket();
        
        void send(const std::string &message);
        std::string listen();

        void setConnect();
        void setReuseAddr();

        struct sockaddr_in getoth_addr();
        void setoth_addr(char *hostname, int port);
};