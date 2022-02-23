#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "../../../include/server/profile_management/session_manager.hpp"
#include "../../../include/server/test_class.hpp"

#define LISTEN_TIMEOUT 10

SessionManager::SessionManager() {
    this->session_closed = this->should_send = this->sent = false;
    listen_thread = std::thread(&SessionManager::_listen, this);
    send_thread = std::thread(&SessionManager::_send, this);
    listen_thread.detach();
    send_thread.detach();
}

bool SessionManager::_session_closed() {
    std::unique_lock<std::mutex> lck(session_mtx);
    bool return_val = session_closed;
    return return_val;
}

void SessionManager::_listen() {
    while(1) {
        // TODO producer-consumer logic
        if (_session_closed()) break;
    }
}

void SessionManager::_send() {
    while(1) {
        std::unique_lock<std::mutex> lck(send_mtx);
        if (should_send) sent = test_obj.send(msg_send);
        should_send = false;

        if (_session_closed()) break;
    }
}

void SessionManager::send(std::string msg) {
    std::unique_lock<std::mutex> lck(send_mtx);
    should_send = true;
    sent = false;
    msg_send = msg;
}

bool SessionManager::msg_sent() {
    std::unique_lock<std::mutex> lck(send_mtx);
    bool confirm = !should_send && sent;
    return confirm;
}

bool SessionManager::close() {
    std::unique_lock<std::mutex> lck(session_mtx);
    session_closed = true;
}

std::string SessionManager::read_buffer() {
    // TODO producer-consumer logic
    return "";
}