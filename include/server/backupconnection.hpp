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
#include "../server/message.hpp"
#include "../server/server.hpp"

class BackupConnection{
    private:

        int id; // server id

        Server *server; // server that is connected with main

        Socket sock; // socket

        std::string send_id; // send thread id
        std::string listen_id; // listen thread id

        int notif_counter; // seqn for notifications

        // connect ack and mutex
        bool connectAck; 
        std::mutex connectAckMutex;

        // getter/setter for ack from backup
        bool getConnectAck();
        void setConnectAck(bool val);

    public:
        BackupConnection(int port, struct sockaddr_in addr, int id, Server *server);
        ~BackupConnection();

        // send loop
        void send();

        // listen loop
        void listen();
};