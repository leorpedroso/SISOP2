#pragma once

#include <string>
#include <mutex>
#include <vector>

#include "../../../include/server/test_class.hpp"

class SessionManager {
    private:
        Test test_obj; // TODO change this to communication class and its methods accordingly
        std::mutex *send_mtx; // TODO change here so mutex is only within session class
        std::mutex session_mtx;
        bool session_closed;
        std::vector<std::string> buffer;
    public:
        SessionManager(std::mutex *send_mtx);
        void listen();
        void send(std::string msg, bool *sent);
        bool close();
        std::string read_buffer();
};