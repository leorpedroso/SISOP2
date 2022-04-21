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

class BackupConnection{
    private:
        Socket sock; // socket

        std::string send_id; // send thread id
        std::string listen_id; // listen thread id

        std::mutex msgs_mtx; // mutex for update_msgs queue
        std::condition_variable msgs_cv; // condition variable that indicates that update_msgs is not empty

        std::queue<std::string> msgs; // queue for update messages

        int notif_counter; // seqn for notifications

        // adds ack message to buffer
        void sendMsg(std::string msg);

    public:
        BackupConnection(int port, struct sockaddr_in addr);
        ~BackupConnection();

        // send loop
        void send();

        // listen loop
        void listen();
};