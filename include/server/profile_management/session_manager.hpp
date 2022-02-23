#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "../../../include/server/test_class.hpp"

class SessionManager {
    private:
        Test test_obj; // TODO change this to communication class and its methods accordingly
        std::mutex send_mtx, listen_mtx, session_mtx;
        std::thread listen_thread, send_thread;
        std::condition_variable listen_cv;
        bool session_closed, should_send, sent;
        std::queue<std::string> buffer;
        std::string msg_send;

        void _listen();
        void _send();
        bool _session_closed();
    public:
        SessionManager();
        void send(std::string msg);
        bool msg_sent();
        bool close();
        std::string read_buffer();
};