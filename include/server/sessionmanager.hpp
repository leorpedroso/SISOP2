#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "../common/socket.hpp"
#include "../server/profile.hpp"

class SessionManager{
    private:
        Socket sock;
        Profile prof;
        bool session_closed;
        std::thread send_thread;
        std::thread listen_thread;
        std::mutex session_mtx;

        bool sessionClosed();
        void closeSession();

    public:
        SessionManager(int port, struct sockaddr_in addr, Profile _prof);
        void send();
        void listen();
};