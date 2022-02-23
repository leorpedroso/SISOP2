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
    session_mtx.lock();
    bool return_val = session_closed;
    session_mtx.unlock();
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
        send_mtx.lock();
        if (should_send) sent = test_obj.send(msg_send);
        should_send = false;
        send_mtx.unlock();

        if (_session_closed()) break;
    }
}

void SessionManager::send(std::string msg) {
    send_mtx.lock(); 
    should_send = true;
    sent = false;
    msg_send = msg;
    send_mtx.unlock();
}

bool SessionManager::msg_sent() {
    send_mtx.lock();
    bool confirm = !should_send && sent;
    send_mtx.unlock();
    return confirm;
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