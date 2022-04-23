#include "../../include/server/servermanager.hpp"

#include "../../include/server/backupconnection.hpp"
#include<iostream>
#include<thread>
#include<mutex>

bool _isServerPrimary;
int _serverID;
int _IDCounter;
std::vector<Server*> _backupServers;
std::mutex _backupServersMutex;

void createServerManager(bool isPrimary){
    _isServerPrimary = isPrimary;
    _serverID = 0;
}

void setServerIDAndCounter(int val){
    _serverID = val;
    _IDCounter = val;
}

void printServers(){
    _backupServersMutex.lock();

    for(Server *server: _backupServers){
        std::cout << "NAME: " << server->getName() << " ID: " << server->getID() << std::endl;
    }

    _backupServersMutex.unlock();
}

void addMessagetoServers(Message msg){
    _backupServersMutex.lock();

    for(Server *server: _backupServers){
        server->addMsg(msg);
    }

    _backupServersMutex.unlock();
}

void addServer(int id, const std::string &name, int port){
    _backupServersMutex.lock();

    Server *server = new Server(id, name, port);

    _backupServers.push_back(server);

    _backupServersMutex.unlock();
}

void addBackupServer(int port, struct sockaddr_in addr){
    _backupServersMutex.lock();

    Server *server = new Server(addr, ++_IDCounter);

    std::string name = server->getName();
    int portServer = server->getPort();
    int id = _IDCounter;

     for(Server *otherServer: _backupServers){
        server->addMsg(Message(Socket::NEW_SERVER, std::to_string(server->getID()) + " " + server->getName() + " " + std::to_string(server->getPort())));
    }

    _backupServers.push_back(server);

    _backupServersMutex.unlock();

    addMessagetoServers(Message(Socket::NEW_SERVER, std::to_string(id) + " " + name + " " + std::to_string(portServer)));
    
    startBackupThreads(addr, port, id, server);
}

void sendBackupThread(std::shared_ptr<BackupConnection> sess){
    sess->send();
}

// Runs the the session manager's listen method. (Required to create a listen thread correctly)
void listenBackupThread(std::shared_ptr<BackupConnection> sess){
    sess->listen();
}

void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server){
    std::shared_ptr<BackupConnection> sess = std::make_shared<BackupConnection>(port, addr, id, server);
    std::thread listen_thread = std::thread(listenBackupThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendBackupThread, sess);
    send_thread.detach();
}