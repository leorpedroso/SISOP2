#pragma once

#include <string>
#include <mutex>
#include <vector>

#include "../../../include/server/test_class.hpp"

class SessionManager {
    private:
        Test test_obj; // TODO change this to communication class and its methods accordingly
        std::mutex send_mtx; 
        std::mutex session_mtx;
        std::thread listen_thread, send_thread;
        bool session_closed, should_send, sent;
        std::vector<std::string> buffer;
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