#include "../../include/server/backupconnection.hpp"
#include "../../include/server/message.hpp"
#include<sstream>
#include<iostream>

BackupConnection::BackupConnection(int port, struct sockaddr_in addr, int id, Server server): sock(port, true), id(id), server(server) {
    sock.setoth_addr(addr);
    sock.setConnect();
}

BackupConnection::~BackupConnection() {
    sock.closeSocket();
}

void BackupConnection::sendMsg(Message msg) {
    std::unique_lock<std::mutex> lck(msgs_mtx);
    msgs.push(msg);
    msgs_cv.notify_one();
}

void BackupConnection::send(){
     // Gets the thread ID
    std::stringstream ss;
    ss << std::this_thread::get_id();
    send_id = ss.str();
    std::cout << "start thread send backup: " << send_id << std::endl;
    sock.send(Socket::CONNECT_OK + " " + std::to_string(id));

    while(1){
        std::unique_lock<std::mutex> lck(msgs_mtx);
        if (msgs_cv.wait_for(lck, std::chrono::microseconds(2), [this]{return !msgs.empty();}) == false)
            continue;        
        Message notification = msgs.front();
        msgs.pop();
        // If it is empty, keep waiting for new messages
        if(notification.getType() == "")
            continue;
        // Sends notification read to the client
        sock.send(Socket::SERVER_UPDATE + " " + notification.getType() + " " + notification.getArgs());
    }
    std::cout << "end thread send backup: " << send_id << std::endl;
}

void BackupConnection::listen(){
    // Gets the thread ID
    std::stringstream ss;
    ss << std::this_thread::get_id();
    listen_id = ss.str();
    std::cout << "start thread listen backup: " << listen_id << std::endl;

    while(1){
        // Listen to a message and check if it isn't empty
        std::string message = sock.listen();
        if (message == "")
            continue;

        // Splits the message to get a valid type of message
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if(spMessage.size() < 2)
            continue;
        std::string type = spMessage[0];

       // TODO LISTEN TO THE MESSAGES
    }
    std::cout << "end thread listen backup: " << listen_id << std::endl;
}