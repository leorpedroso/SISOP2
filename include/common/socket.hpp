#pragma once

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <stdio.h>
#include <vector>

class Socket{
    private:
        int sockfd;
        struct sockaddr_in addr;
        struct sockaddr_in oth_addr;
        socklen_t clilen;
        bool connected;
        
    public:
        const static std::string CONNECT; // profile
        const static std::string CONNECT_OK;
        const static std::string CONNECT_NOT_OK;
        const static std::string EXIT;
        const static std::string FOLLOW;
        const static std::string SEND_NOTIFICATION;
        const static std::string NOTIFICATION;
        const static int MAX_MESSAGE_SIZE;

        Socket(int port = 0);
        void closeSocket();
        
        void send(const std::string &message);
        std::string listen();

        void setConnect();
        void setReuseAddr();

        struct sockaddr_in getoth_addr();
        void setoth_addr(char *hostname, int port);

        static std::vector<std::string> splitMessage(const std::string &message);
        static std::vector<std::string> splitUpToMessage(const std::string &message, int n);
        static std::string getTypeMessage(const std::string &message);
};