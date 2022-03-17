#include "../../include/server/sessionmanager.hpp"
#include <iostream>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/notification.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>

// Handle session from each client from their profiles
SessionManager::SessionManager(int port, struct sockaddr_in addr, Profile *_prof): prof(_prof), sock(port, true), addr(addr){
    sock.setoth_addr(addr);
    sock.setConnect();
    profileName = prof->getName();
    prof->incrementSessions();
    session_closed = false;
    notif_counter = 0;    
}

// Sends notifications to a client
void SessionManager::send(){    
    // Gets a valid ID
    std::stringstream ss;
    ss << std::this_thread::get_id();
    send_id = ss.str();
    std::cout << "start thread send: " << send_id << std::endl;
    sock.send(Socket::CONNECT_OK + " " + send_id);

    while(1){
        // If the session was closed, ends the send
        if (sessionClosed()) break;
        verifySendAck();

        // Reads an notification waiting to be send to the client
        if(prof->canRead(send_id)) {
            Notification notification = prof->readNotification(send_id);
            // If it is empty, keep reading messages
            if(notification.getMessage() == "")
                continue;
            // Sends notification from the send to the client
            std::cout << "thread: " << send_id << " FROM " << notification.getSender() << " TO " << prof->getName() << " MSG: " << notification.getMessage() << std::endl;
            sock.send(Socket::NOTIFICATION + " " + std::to_string(notif_counter) + " "  + notification.getSender() + " " + notification.getTime() + " " + notification.getMessage());
            notif_counter++;
        }
    }
    std::cout << "end thread send: " << send_id << std::endl;
}

// Listen to notification from a client
void SessionManager::listen(){
    // Gets a valid ID
    std::stringstream ss;
    ss << std::this_thread::get_id();
    listen_id = ss.str();
    std::cout << "start thread listen: " << listen_id << std::endl;

    while(1){
        // Listen to a message and check if it isn't empty
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::string receiveTimeString = getTime();

        // Splits the message to get a valid type of message
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 3);
        if(spMessage.size() < 3)
            continue;
        std::string type = spMessage[0];

        // If the type is EXIST, close the client session 
        if (type == Socket::EXIT){
            std::cout<< "thread: " << listen_id  << " EXIT" << std::endl;
            std::string prof = spMessage[1];
            std::string sess = spMessage[2];
            closeSession();
            getProfile(prof)->decrementSessions();
            break;
        // If the type is FOLLOW, add the follower profile to the list if the following profile exists
        } else if (type == Socket::FOLLOW){
            std::cout<< "thread: " << listen_id  << " FOLLOW" << std::endl;
            std::string prof = spMessage[1];
            std::string foll = spMessage[2];
            Profile *folProf = getProfile(foll);

            if (folProf == nullptr){
                sendAck("FOLLOW 0 " + foll);
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                bool alreadyFollows = folProf->addFollower(prof, true);
                if (alreadyFollows) sendAck("FOLLOW 2 " + foll);
                else sendAck("FOLLOW 1 " + foll);
            }
        // If the type is SEND, send the message to all the followers clients with open sessions
        } else if (type == Socket::SEND_NOTIFICATION){
            std::cout<< "thread: " << listen_id  << " SEND_NOTIFICATION" << std::endl;
            std::string prof = spMessage[1];
            std::string msg = spMessage[2];
            sendAck("SEND " + receiveTimeString + " " + msg);
            getProfile(prof)->notifyFollowers(msg, receiveTimeString);
        // If the type isn't recognize, outputs an error message
        } else {
            std::cout<< "thread: " << listen_id << " ERROR " + message << std::endl;
        }
    }
    std::cout << "end thread listen: " << listen_id << std::endl;
}

// Checks if a session to closed, uses mutex to avoid wrong data
bool SessionManager::sessionClosed() {
    std::unique_lock<std::mutex> lck(session_mtx);
    bool returnVal = session_closed;
    return returnVal;
}

// Closes a session, uses mutex to avoid wrong data
void SessionManager::closeSession() {
    std::unique_lock<std::mutex> lck(session_mtx);
    session_closed = true;
}

// Put a message in the ack to send it, uses mutex to avoid miss data
void SessionManager::sendAck(std::string msg) {
    std::unique_lock<std::mutex> lck(ack_mtx);
    ack_msgs.push(msg);
    ack_cv.notify_one();
}

// Checks if there is an message to be send through socket and sends it
// Uses mutex to avoid miss data
void SessionManager::verifySendAck() {
    std::unique_lock<std::mutex> lck(ack_mtx);
    if (ack_cv.wait_for(lck, std::chrono::microseconds(2), [this]{return !ack_msgs.empty();}) == false)
        return;
    std::string msg = ack_msgs.front();
    ack_msgs.pop();
    sock.send(Socket::ACK + " " + msg);
}

// Returns the actual local time based on the server computer
std::string SessionManager::getTime() {
    auto receiveTime = std::chrono::system_clock::now();
    std::time_t rec_time = std::chrono::system_clock::to_time_t(receiveTime);
    std::stringstream bufferTime;
    bufferTime << std::put_time(std::localtime(&rec_time), "%d/%b/%Y-%H:%M:%S");
    return bufferTime.str();
}