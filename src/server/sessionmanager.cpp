#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"


extern std::unordered_map<std::string, Profile> _profiles;

int gambiarra = 0;

SessionManager::SessionManager(int port, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(port, true){
    sock.setoth_addr(addr);
    sock.setConnect();

    prof->incrementSessions();

    session_closed = false;

    sock.send(sock.CONNECT_OK);

    send_thread = std::thread(&SessionManager::send, this);
    std::thread::id thread_id = send_thread.get_id();
    std::cout<< "start thread_id:" << thread_id << std::endl;
    send_thread.detach();

    listen_thread = std::thread(&SessionManager::listen, this);
    listen_thread.detach();

    std::thread::id thread_id2 = send_thread.get_id();
    std::cout<< "start thread_id:" << thread_id2 << std::endl;
}

void SessionManager::send(){    
    ++gambiarra;
    int valorSalvo = gambiarra;
    std::thread::id thread_id = send_thread.get_id();
    std::cout << valorSalvo << "start thread_id:" << thread_id << std::endl;

    std::cout << "1testedefinitivo: " << prof->getName() << " " << prof << std::endl; 

    while(1){
        if (sessionClosed()) break;
        if(prof->canRead(thread_id)) {
            for(auto &any: _profiles) {
                Profile profile = any.second;

                std::vector<std::string> followers = profile.getFollowers();
                std::cout << any.second.getName() << std::endl;

                for(const std::string &follower: followers){
                    std::cout << " " << follower << std::endl;
                }

                std::cout << "\n" << std::endl;
            }

            //Profile *tempProf = getProfile("dafa2");
            Notification notification = prof->readNotification(thread_id);
            std::cout << valorSalvo << thread_id << prof->getName() << " " << notification.getSender() + " " + notification.getMessage() << std::endl;
            std::cout << "2testedefinitivo: " << prof->getName() << " " << prof << std::endl; 
            sock.send(sock.NOTIFICATION + " " + notification.getSender() + " " + notification.getMessage());
        }
    }
}

void SessionManager::listen(){
    std::cout << "beg smanager prof: " << prof->getName() << " " << std::endl; 
    while(1){
        std::string message = sock.listen();
        std::string type = sock.getTypeMessage(message);

        if (type == sock.EXIT){
            std::cout << "EXIT" << std::endl;
            closeSession();
            prof->decrementSessions();
            break;
        } else if (type == sock.FOLLOW){
            std::cout << "FOLLOW" << std::endl;
            std::string follow = sock.splitUpToMessage(message, 2)[1];
            Profile *folProf = getProfile(follow);
            if (folProf == nullptr){
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                folProf->addFollower(prof->getName(), true);
            }
        } else if (type == sock.SEND_NOTIFICATION){
            std::cout << "SEND_NOTIFICATION" << std::endl;
            std::cout << "prof: " << prof->getName() << std::endl;
            prof->notifyFollowers(sock.splitUpToMessage(message, 2)[1]);
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
