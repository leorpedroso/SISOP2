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


void Server::addMsg(Message msg){
    msgs_mtx.lock();
    msgs.push(msg);
    msgs_cv.notify_all();
    msgs_mtx.unlock();
}

Message Server::popMsg(){
    std::unique_lock<std::mutex> mlock(msgs_mtx);

    if (msgs_cv.wait_for(mlock, std::chrono::microseconds(2), [this]{return !msgs.empty();}) == false)
        return Message("",""); 

    Message msg = msgs.front();
    msgs.pop();

    return msg;
}

