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
#include "../server/profile.hpp"
#include "../server/profilemanager.hpp"

class SessionManager{
    private:
        Socket sock; // socket
        Profile *prof; // profile

        std::string profileName; // profile name

        std::string send_id; // send thread id
        std::string listen_id; // listen thread id

        struct sockaddr_in addr; // addr

        bool session_closed; // boolean for checking if the session is closed

        std::mutex session_mtx; // mutex for session_closed variable
        std::mutex ack_mtx; // mutex for ack_msgs queue

        std::condition_variable ack_cv; // condition variable that indicates that ack_msgs is not empty

        std::queue<std::string> ack_msgs; // queue for ack messages

        int notif_counter; // seqn for notifications

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

    public:
        SessionManager(int port, struct sockaddr_in addr, Profile *_prof);

        // send loop
        void send();

        // listen loop
        void listen();
};