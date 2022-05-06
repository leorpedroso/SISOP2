#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "../common/socket.hpp"
#include "../server/ack.hpp"
#include "../server/profile.hpp"
#include "../server/profilemanager.hpp"

class SessionManager{
    private:
        Socket sock; // socket
        Profile *prof; // profile

        // profile name and mutex
        std::string profileName; 
        std::mutex profileNameMutex;

        std::string send_id; // send thread id
        std::string listen_id; // listen thread id

        struct sockaddr_in addr; // addr

        // addr string and mutex
        std::string addrString;
        std::mutex addrStringMutex;

        bool session_closed; // boolean for checking if the session is closed

        std::mutex session_mtx; // mutex for session_closed variable
        std::mutex ack_mtx; // mutex for ack_msgs queue

        std::condition_variable ack_cv; // condition variable that indicates that ack_msgs is not empty

        std::queue<Ack> ack_msgs; // queue for ack messages

        int notif_counter; // seqn for notifications

        // connect ack and mutex
        bool connectAck; 
        std::mutex connectAckMutex;

        // checks if a sessions is closed
        bool sessionClosed();

        // closes session
        void closeSession();

        // adds ack message to buffer
        void sendAck(std::string msg);

        // sends ack message
        void verifySendAck();

        // returns current time
        std::string getTime();

        // getters for string and name with mutexes
        const std::string &getAddrString();
        const std::string &getProfileName();

        // getter/setter for ack from client
        bool getConnectAck();
        void setConnectAck(bool val);

    public:
        SessionManager(int port, struct sockaddr_in addr, Profile *_prof);
        ~SessionManager();

        // send loop
        void send();

        // listen loop
        void listen();
};