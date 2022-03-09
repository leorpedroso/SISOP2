#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"


SessionManager::SessionManager(int port, struct sockaddr_in addr, Profile _prof, ProfileManager manager): prof(_prof), sock(port, true), manager(manager) {
    sock.setoth_addr(addr);
    sock.setConnect();

    prof.incrementSessions();

    sock.send(sock.CONNECT_OK);

    send_thread = std::thread(&SessionManager::send, this);
    send_thread.detach();

    listen_thread = std::thread(&SessionManager::listen, this);
    listen_thread.detach();
}

void SessionManager::send(){    
    std::thread::id thread_id = send_thread.get_id();

    while(1){
        if (sessionClosed()) break;
        if(prof.canRead(thread_id))
            sock.send(sock.NOTIFICATION + " " + prof.readNotification(thread_id).getMessage());
    }
}

void SessionManager::listen(){
    while(1){
        std::string message = sock.listen();
        std::string type = sock.getTypeMessage(message);

        if (type == sock.EXIT){
            closeSession();
            prof.decrementSessions();
            break;
        } else if (type == sock.FOLLOW){
            std::string follower = sock.splitUpToMessage(message, 2)[1];
            std::shared_ptr<Profile> folProf = manager.getProfile(follower);
            if (folProf == nullptr){
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                prof.addFollower(*folProf);
            }
        } else if (type == sock.SEND_NOTIFICATION){
            for (auto fol:prof.getFollowers()){
                fol.putNotification(sock.splitUpToMessage(message, 2)[1]);
            }
        } else {
            std::cout << "ERROR " + message << std::endl;
        }
    }
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
