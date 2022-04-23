#include "../../include/server/server.hpp"

const std::string &Server::getName() const{
    return name;
}

const int &Server::getID() const{
    return ID;
}

const int &Server::getPort() const{
    return port;
}

const struct sockaddr_in &Server::getAddr() const{
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

