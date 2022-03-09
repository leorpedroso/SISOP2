#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"


extern std::unordered_map<std::string, Profile> _profiles;

SessionManager::SessionManager(int port, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(port, true){
    sock.setoth_addr(addr);
    sock.setConnect();

    prof->incrementSessions();

    session_closed = false;

    sock.send(sock.CONNECT_OK);

    send_thread = std::thread(&SessionManager::send, this);
    send_id = send_thread.get_id();
    send_thread.detach();

    listen_thread = std::thread(&SessionManager::listen, this);
    listen_id = listen_thread.get_id();
    listen_thread.detach();
}

void SessionManager::send(){    
    std::cout << "start thread send: " << send_id << std::endl;

    while(1){
        if (sessionClosed()) break;
        if(prof->canRead(send_id)) {
            Notification notification = prof->readNotification(send_id);
            std::cout << "thread: " << send_id << " FROM " << notification.getSender() << " TO " << prof->getName() << " MSG: " << notification.getMessage() << std::endl;
            sock.send(sock.NOTIFICATION + " " + notification.getSender() + " " + notification.getMessage());
        }
    }

    std::cout << "end thread send: " << send_id << std::endl;
}

void SessionManager::listen(){
    std::cout << "start thread listen: " << listen_id << std::endl;

    while(1){
        std::string message = sock.listen();
        std::string type = sock.getTypeMessage(message);

        if (type == sock.EXIT){
            std::cout << "thread: " <<  listen_id << " EXIT" << std::endl;
            closeSession();
            prof->decrementSessions();
            break;
        } else if (type == sock.FOLLOW){
            std::cout << "thread: " <<  listen_id << " FOLLOW" << std::endl;
            std::string follow = sock.splitUpToMessage(message, 2)[1];
            Profile *folProf = getProfile(follow);
            if (folProf == nullptr){
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                folProf->addFollower(prof->getName(), true);
            }
        } else if (type == sock.SEND_NOTIFICATION){
            std::cout << "thread: " <<  listen_id << " SEND_NOTIFICATION" << std::endl;
            std::cout << "prof: " << prof->getName() << std::endl;
            prof->notifyFollowers(sock.splitUpToMessage(message, 2)[1]);
        } else {
            std::cout << "thread: " <<  listen_id << " ERROR " + message << std::endl;
        }
    }
    std::cout << "end thread listen: "<< listen_id << std::endl;
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
