#pragma once

#include <string>
#include <thread>
#include <mutex>

#include "../test_class.hpp"
#include "message_listener.hpp"

class SessionManager : public MessageListener {
    private:
        std::thread send_thread;
        std::mutex send_mtx;
        bool should_send, sent;
        std::string msg_send;

        void _send();

    public:
        SessionManager(std::string clientName);
        void send(std::string msg);
        bool msg_sent();
};