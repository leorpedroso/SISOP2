#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>


SessionManager::SessionManager(int port, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(port, true), addr(addr){
    sock.setoth_addr(addr);
    sock.setConnect();

    profileName = prof->getName();

    prof->incrementSessions();

    session_closed = false;
    
}

void SessionManager::send(){    
    std::stringstream ss;
    ss << std::this_thread::get_id();
    send_id = ss.str();

    std::cout << "start thread send: " << send_id << std::endl;

    sock.send(Socket::CONNECT_OK + " " + send_id);

    while(1){
        if (sessionClosed()) break;

        verifySendAck();

        if(prof->canRead(send_id)) {
            Notification notification = prof->readNotification(send_id);
            if(notification.getMessage() == "")
                continue;
            std::cout << "thread: " << send_id << " FROM " << notification.getSender() << " TO " << prof->getName() << " MSG: " << notification.getMessage() << std::endl;
            sock.send(Socket::NOTIFICATION + " " + notification.getSender() + " " + notification.getTime() + " " + notification.getMessage());
        }
    }

    std::cout << "end thread send: " << send_id << std::endl;
}

void SessionManager::listen(){
    std::stringstream ss;
    ss << std::this_thread::get_id();
    listen_id = ss.str();

    std::cout << "start thread listen: " << listen_id << std::endl;

    while(1){
        std::string message = sock.listen();

        if (message == "")
            continue;

        auto receiveTime = std::chrono::system_clock::now();
        std::time_t rec_time = std::chrono::system_clock::to_time_t(receiveTime);
        std::stringstream bufferTime;
        bufferTime << std::put_time(std::localtime(&rec_time), "%d/%b/%Y-%H:%M:%S");
        std::string receiveTimeString = bufferTime.str();

        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 3);
        std::string type = spMessage[0];

        if (type == Socket::EXIT){
            std::cout<< "thread: " << listen_id  << " EXIT" << std::endl;
            std::string prof = spMessage[1];
            std::string sess = spMessage[2];

            closeSession();
            getProfile(prof)->decrementSessions();
            break;
        } else if (type == Socket::FOLLOW){
            std::cout<< "thread: " << listen_id  << " FOLLOW" << std::endl;

            std::string prof = spMessage[1];
            std::string foll = spMessage[2];

            Profile *folProf = getProfile(foll);
            if (folProf == nullptr){
                sendAck("FOLLOW 0");
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                sendAck("FOLLOW 1");
                folProf->addFollower(prof, true);
            }

        } else if (type == Socket::SEND_NOTIFICATION){
            std::cout<< "thread: " << listen_id  << " SEND_NOTIFICATION" << std::endl;

            std::string prof = spMessage[1];
            std::string msg = spMessage[2];

            sendAck("SEND");

            getProfile(prof)->notifyFollowers(msg, receiveTimeString);
        } else {
            std::cout<< "thread: " << listen_id << " ERROR " + message << std::endl;
        }
    }
    std::cout << "end thread listen: " << listen_id << std::endl;
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

void SessionManager::sendAck(std::string msg) {
    std::unique_lock<std::mutex> lck(ack_mtx);
    ack_msgs.push(msg);
    ack_cv.notify_one();
}

void SessionManager::verifySendAck() {
    std::unique_lock<std::mutex> lck(ack_mtx);
    if (ack_cv.wait_for(lck, std::chrono::microseconds(2), [this]{return !ack_msgs.empty();}) == false)
        return;
    std::string msg = ack_msgs.front();
    ack_msgs.pop();
    sock.send(Socket::ACK + " " + msg);
}