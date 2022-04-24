#pragma once

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <vector>

class Socket{
    private:

        // internal socket variables
        int sockfd;
        struct sockaddr_in oth_addr;
        socklen_t clilen;

        // boolean variables
        bool log; // indicates if messages should be printed on screen when sent
        bool connected; // indicates if the socket is connected
        
    public:
        // constants for message types
        const static std::string CONNECT; // 0 <profile>
        const static std::string CONNECT_OK; // 1 <session_id>
        const static std::string CONNECT_NOT_OK; // 2 <error_message>
        const static std::string EXIT; // 3 <profile_name> <session_id>
        const static std::string FOLLOW; // 4 <follower> <profile>
        const static std::string SEND_NOTIFICATION; // 5 <profile> <message>
        const static std::string NOTIFICATION; // 6 <seqn> <sender> <time> <message>
        const static std::string ACK; // 7 <ack_type> <args>
        const static std::string CONNECT_SERVER; // 8
        const static std::string SERVER_UPDATE; // 9 ?
        const static std::string ALIVE; // 10 ?
        const static std::string SERVER_ACK; // 11 ?
        const static std::string NEW_SERVER; // 12 ?

        // max size for a socket message
        const static int MAX_MESSAGE_SIZE;

        Socket(int port = 0, bool reusePort = false, bool log = true);

        // closes socket
        void closeSocket();

        // sends messages 
        void send(const std::string &message);
        void send(const std::string &message, struct sockaddr_in addr);

        // listens for messages from socket
        std::string listen();
        std::string listen(struct sockaddr_in &addr);

        // sets connect
        void setConnect();

        // sets reuseport
        void setReusePort();

        // disables logging
        void disableLog();

        // getter and setters for the communicating address and port
        struct sockaddr_in getoth_addr();
        void setoth_addr(char *hostname, int port);
        void setoth_addr(struct sockaddr_in new_addr);
        static std::string get_addr_string(struct sockaddr_in addr);

        // split messages 
        static std::vector<std::string> splitMessage(const std::string &message);
        static std::vector<std::string> splitUpToMessage(const std::string &message, int n);
        static std::string getTypeMessage(const std::string &message);
};