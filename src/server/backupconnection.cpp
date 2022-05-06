#include "../../include/server/backupconnection.hpp"
#include "../../include/server/message.hpp"
#include "../../include/server/servermanager.hpp"
#include<sstream>
#include<iostream>

BackupConnection::BackupConnection(int port, struct sockaddr_in addr, int id, Server *server): sock(port, true), id(id), server(server) {
    connectAck = false;
    sock.setoth_addr(addr);
    sock.setConnect();
}

BackupConnection::~BackupConnection() {
    sock.closeSocket();
}

void BackupConnection::send(){
     // Gets the thread ID
    std::stringstream ss;
    ss << std::this_thread::get_id();
    send_id = ss.str();
    std::cout << "start thread send backup: " << send_id << std::endl;

    // sends backup server message confirming connection
    sock.send(Socket::CONNECT_SERVER_OK + " " + std::to_string(id));

    while(1){

        if(!getConnectAck()) continue;

        Message notification = server->popMsg();
        // If it is empty, keep waiting for new messages
        if(notification.getType() == "")
            continue;
        // Sends notification read to the backup server
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
        
        if(type == Socket::ALIVE){
            // backup server checking if main server is alive
            server->addMsg(Message(Socket::ALIVE, "Alive"), nullptr);
        } else if(type == Socket::SERVER_ACK){
            // backup server confirming a message was received
            int id = stoi(spMessage[1]);

            auto count = getCounterFromMap(id);
            if(count != nullptr){
                count->decrementValue();
                if(count->getValue() == 0){
                    removeCounterFromMap(id);
                }
            }
            // Ack for server connect
        } else if(type == Socket::CONNECT_ACK){
            setConnectAck(true);
        }
    }
    std::cout << "end thread listen backup: " << listen_id << std::endl;
}

// getter/setter for ack from client
bool BackupConnection::getConnectAck(){
    std::unique_lock<std::mutex> mlock(connectAckMutex);
    return connectAck;
}
void BackupConnection::setConnectAck(bool val){
    std::unique_lock<std::mutex> mlock(connectAckMutex);
    connectAck = val;
}