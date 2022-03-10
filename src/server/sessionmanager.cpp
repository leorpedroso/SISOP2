#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"
#include <sstream>


SessionManager::SessionManager(Socket *sock, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(sock), addr(addr){

    profileName = prof->getName();

    prof->incrementSessions();

    session_closed = false;
}

void SessionManager::send(){    
    std::stringstream ss;
    ss << std::this_thread::get_id();
    send_id = ss.str();

    putSession(send_id, this);
    putSessionAddr(send_id, &addr);

    std::cout << "start thread send: " << send_id << std::endl;

    sock->send(Socket::CONNECT_OK + " " + send_id, addr);

    while(1){
        if (sessionClosed()) break;

        if(prof->canRead(send_id)) {
            Notification notification = prof->readNotification(send_id);
            if(notification.getMessage() == "")
                continue;
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
std::unordered_map<std::string, struct sockaddr_in *> _sessionAddr;
std::mutex _sessionsAddrMutex;
std::mutex _sessionsMutex;

void putSession(const std::string &id, SessionManager* man){
    std::unique_lock<std::mutex> mlock(_sessionsMutex);
    _sessions.insert({id, man});
}

void putSessionAddr(const std::string &id, struct sockaddr_in *addr){
    std::unique_lock<std::mutex> mlock(_sessionsAddrMutex);
    _sessionAddr.insert({id, addr});
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

struct sockaddr_in *getSessionAddr(const std::string &name){
    std::unique_lock<std::mutex> mlock(_sessionsAddrMutex);

    auto pos = _sessionAddr.find(name);
    if(pos == _sessionAddr.end()){
         return nullptr;
    } else {
        return pos->second;
    }
}

