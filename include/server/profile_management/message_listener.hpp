#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../test_class.hpp"


class MessageListener {
    private:
        std::thread listen_thread;
        std::mutex listen_mtx, session_mtx;
        std::queue<std::string> buffer;
        std::condition_variable listen_cv;
        bool session_closed;

        void _listen();

    protected:
        Test test_obj; // TODO change this to communication class and its methods accordingly
        
        bool _session_closed();

    public:
        MessageListener(std::string clientName);
        std::string read_buffer();
        void close();  
};