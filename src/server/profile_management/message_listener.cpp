#include <string>
#include <thread>
#include <mutex>

#include "../../../include/server/profile_management/message_listener.hpp"
#include "../../../include/server/test_class.hpp"

#define LISTEN_TIMEOUT 10

MessageListener::MessageListener(std::string clientName) : test_obj(clientName) {
    session_closed = false;
    listen_thread = std::thread(&MessageListener::_listen, this);
    listen_thread.detach();
}


void MessageListener::_listen() {
    while(1) {
        if (_session_closed()) break;

        std::string msg_receive = test_obj.receive(LISTEN_TIMEOUT);    
        if (msg_receive.compare("") != 0) {
            std::unique_lock<std::mutex> lck(listen_mtx);
            buffer.push(msg_receive);
            listen_cv.notify_one();
        }
    }
}


bool MessageListener::_session_closed() {
    std::unique_lock<std::mutex> lck(session_mtx);
    bool return_val = session_closed;
    return return_val;
}


void MessageListener::close() {
    std::unique_lock<std::mutex> lck(session_mtx);
    session_closed = true;
}


std::string MessageListener::read_buffer() {
    std::unique_lock<std::mutex> lck(listen_mtx);
    if (listen_cv.wait_for(lck, std::chrono::microseconds(2), [this]{return !buffer.empty();}) == false)
        return "";
    std::string msg_buffer = buffer.front();
    buffer.pop();
    return msg_buffer;
}
