#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"
#include <sstream>


extern std::unordered_map<std::string, Profile> _profiles;

SessionManager::SessionManager(Socket *sock, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(sock), addr(addr){

    prof->incrementSessions();

    session_closed = false;

    send_thread = std::thread(&SessionManager::send, this);
    std::stringstream ss;
    ss << send_thread.get_id();
    send_id = ss.str();
    std::cout << send_thread.get_id() << std::endl;

    send_thread.detach();
}

void SessionManager::send(){    
    std::cout << "start thread send: " << send_id << std::endl;
    putSession(send_id, this);
    sock->send(Socket::CONNECT_OK + " " + send_id, addr);

    while(1){
        if (sessionClosed()) break;
        if(prof->canRead(send_id)) {
            Notification notification = prof->readNotification(send_id);
            std::cout << "thread: " << send_id << " FROM " << notification.getSender() << " TO " << prof->getName() << " MSG: " << notification.getMessage() << std::endl;
            sock->send(Socket::NOTIFICATION + " " + notification.getSender() + " " + notification.getMessage(), addr);
        }
    }

    std::cout << "end thread send: " << send_id << std::endl;
}

bool SessionManager::sessionClosed() {
    std::unique_lock<std::mutex> lck(session_mtx);
    bool returnVal = session_closed;
    return returnVal;
}


void SessionManager::closeSession() {
    std::unique_lock<std::mutex> lck(session_mtx);
    session_closed = true;
}


std::unordered_map<std::string, SessionManager*> _sessions;
std::mutex _sessionsMutex;

void putSession(const std::string &id, SessionManager* man){
    std::unique_lock<std::mutex> mlock(_sessionsMutex);
    _sessions.insert({id, man});
}

SessionManager *getSession(const std::string &name){
    std::unique_lock<std::mutex> mlock(_sessionsMutex);

    auto pos = _sessions.find(name);
    if(pos == _sessions.end()){
         return nullptr;
    } else {
        return pos->second;
    }
}