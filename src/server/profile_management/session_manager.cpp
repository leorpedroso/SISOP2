#include <string>
#include <thread>
#include <mutex>

#include "../../../include/server/profile_management/session_manager.hpp"
#include "../../../include/server/test_class.hpp"

SessionManager::SessionManager(std::string clientName) : MessageListener(clientName) {
    this->should_send = this->sent = false;
    send_thread = std::thread(&SessionManager::_send, this);
    send_thread.detach();
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
