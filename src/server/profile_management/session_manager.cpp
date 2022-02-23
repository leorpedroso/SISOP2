#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "../../../include/server/profile_management/session_manager.hpp"
#include "../../../include/server/test_class.hpp"

#define LISTEN_TIMEOUT 10

SessionManager::SessionManager(std::mutex *send_mtx) {
    this->send_mtx = send_mtx;
    this->session_closed = false;
}

void SessionManager::listen() {
    while(1) {
        // TODO producer-consumer logic
        session_mtx.lock();
        if (session_closed) break;
        session_mtx.unlock();
    }
}

void SessionManager::send(std::string msg, bool *sent) {
    send_mtx->lock(); 
    *sent = test_obj.send(msg);
    send_mtx->unlock();
}

bool SessionManager::close() {
    session_mtx.lock();
    session_closed = true;
    session_mtx.unlock();
}

std::string SessionManager::read_buffer() {
    // TODO producer-consumer logic
    return "";
}