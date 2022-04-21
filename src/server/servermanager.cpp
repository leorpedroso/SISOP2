#include "../../include/server/servermanager.hpp"

#include "../../include/server/backupconnection.hpp"
#include<iostream>
#include<thread>

bool isServerPrimary;
unsigned int serverID;
unsigned int IDCounter;
std::vector<Server> backupServers;

void createServerManager(bool isPrimary){
    isServerPrimary = isPrimary;
    if (isPrimary){
        serverID = 0;
    } else {
        serverID = -1;
    }
}

void printServers(){
    for(Server server: backupServers){
        std::cout << "NAME: " << server.getName() << " ID: " << server.getID() << std::endl;
    }
}

void addBackupServer(struct sockaddr_in addr, int port){
    backupServers.push_back(Server(get_addr_string(addr), ++IDCounter));
    startBackupThreads(server, port);
}

void sendBackupThread(std::shared_ptr<BackupConnection> sess){
    sess->send();
}

// Runs the the session manager's listen method. (Required to create a listen thread correctly)
void listenBackupThread(std::shared_ptr<BackupConnection> sess){
    sess->listen();
}

void startBackupThreads(struct sockaddr_in addr, int port){
    std::shared_ptr<BackupConnection> sess = std::make_shared<BackupConnection>(port, addr);
    std::thread listen_thread = std::thread(listenBackupThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendBackupThread, sess);
    send_thread.detach();
}