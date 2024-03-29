#include "../../include/server/server.hpp"

const std::string &Server::getName(){
    std::unique_lock<std::mutex> mlock(name_mtx);
    return name;
}

const int &Server::getID(){
    std::unique_lock<std::mutex> mlock(id_mtx);
    return ID;
}

const int &Server::getPort(){
    std::unique_lock<std::mutex> mlock(port_mtx);
    return port;
}

const struct sockaddr_in &Server::getAddr(){
    std::unique_lock<std::mutex> mlock(addr_mtx);
    return addr;
}

// adds message to backup server queue
void Server::addMsg(Message msg, std::shared_ptr<Counter> counter){
    msgs_mtx.lock();
    msgs.push(msg);
    msgs_counters.push(counter);
    msgs_cv.notify_all();
    msgs_mtx.unlock();
}

// pop message from queue
Message Server::popMsg(){
    std::unique_lock<std::mutex> mlock(msgs_mtx);

    if (msgs_cv.wait_for(mlock, std::chrono::microseconds(2), [this]{return !msgs.empty();}) == false)
        return Message("",""); 

    // no counters currently
    if(msgs_counters_send.empty()){
        // nothing to do 
    // current counter is nullptr
    } else if(msgs_counters_send.front() == nullptr){
        msgs_counters_send.pop();
    // current counter did not receive acks
    } else if (msgs_counters_send.front()->getValue() != 0){
        return Message("",""); 
    // current counter received acks
    } else {
        msgs_counters_send.pop();
    }

    Message msg = msgs.front();
    msgs.pop();
    msgs_counters_send.push(msgs_counters.front());
    msgs_counters.pop();

    return msg;
}

