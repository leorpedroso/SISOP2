#include "../../include/server/servermanager.hpp"

#include "../../include/server/backupconnection.hpp"
#include "../../include/server/profilemanager.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include<memory>
#include"../../include/server/counter.hpp"

// server id
int _serverID;
std::mutex _serverIDMutex;
// counter used to give ids to new servers
int _IDCounter;
// indicates if server is primary
bool _isServerPrimary;
// ports 
int _secPort;
int _tercPort;

// backup servers list
std::vector<Server *> _backupServers;
std::mutex _backupServersMutex;

// map from ids to Countes. 
// used for confirming an ack from a backup server
std::unordered_map<int, std::shared_ptr<Counter>> _counterMap;
std::mutex _counterMapMutex;

// gives an id to every message sent to backup servers
int _globalMessageCount;
std::mutex _globalMessageCountMutex;

// adds counter to map
void addCounterToMap(int id, std::shared_ptr<Counter> count){
    std::unique_lock<std::mutex> mlock(_counterMapMutex);
    _counterMap[id] = count;
}

// returns counter from map
std::shared_ptr<Counter> getCounterFromMap(int id){
    std::unique_lock<std::mutex> mlock(_counterMapMutex);
    auto pos = _counterMap.find(id);
    if(pos != _counterMap.end()){
        return pos->second;
    } else{
        return nullptr;
    }
}

// removes counter from map
void removeCounterFromMap(int id){
    std::unique_lock<std::mutex> mlock(_counterMapMutex);

    auto pos = _counterMap.find(id);
    if(pos != _counterMap.end()){
        _counterMap.erase(id);
    }
}


// increments and returns global counter
int getGlobalMessageCount(){
    std::unique_lock<std::mutex> mlock(_globalMessageCountMutex);

    return ++_globalMessageCount;
}

// changes global counter value (only used by backup servers)
// used to resync counter value from backup servers
void setGlobalMessageCount(int count){
    std::unique_lock<std::mutex> mlock(_globalMessageCountMutex);
    _globalMessageCount = count;
}

// initializes server manager variables
void createServerManager(bool isPrimary) {
    _isServerPrimary = isPrimary;
    _globalMessageCount = 0;
    _serverID = 0;
}

// setters/getters for ports
void setTercPort(int port){
    _tercPort = port;
}
int getTercPort(){
    return _tercPort;
}
void setSecPort(int port){
    _secPort = port;
}
int getSecPort(){
    return _secPort;
}

// setter/getter for serverID and counter
void setServerIDAndCounter(int val) {
    std::unique_lock<std::mutex> mlock(_serverIDMutex);
    _serverID = val;
    _IDCounter = val;
}
int getServerID() {
    std::unique_lock<std::mutex> mlock(_serverIDMutex);
    return _serverID;
}

// getter for number of servers
int getNumberServers(){
    std::unique_lock<std::mutex> mlock(_backupServersMutex);

    return _backupServers.size();
}

// prints current servers
void printServers() {
    _backupServersMutex.lock();

    for (Server *server : _backupServers) {
        std::cout << "NAME: " << server->getName() << " ID: " << server->getID() << std::endl;
    }

    _backupServersMutex.unlock();
}

// adds a message to all servers
void addMessagetoServers(Message msg) {
    _backupServersMutex.lock();

    for (Server *server : _backupServers) {
        server->addMsg(msg);
    }

    _backupServersMutex.unlock();
}

// removes a backup server
void removeFromBackupServers(int id) {
    _backupServersMutex.lock();

    int pos = -1;

    for (auto i = _backupServers.begin(); i != _backupServers.end(); ++i) {
        if ((*i)->getID() == id) {
            _backupServers.erase(i);
            break;
        }
    }

    _backupServersMutex.unlock();
}

// adds a new server
void addServer(int id, const std::string &name, int port) {
    _backupServersMutex.lock();

    Server *server = new Server(id, name, port);

    _backupServers.push_back(server);

    _backupServersMutex.unlock();
}

// returns backup servers
std::vector<Server *> getBackupServers() {
    std::unique_lock<std::mutex> mlock(_backupServersMutex);
    return _backupServers;
}

// adds new backup server
void addBackupServer(int port, struct sockaddr_in addr) {
    _backupServersMutex.lock();

    // creates server
    Server *server = new Server(addr, ++_IDCounter);

    std::string name = server->getName();
    int portServer = server->getPort();
    int id = _IDCounter;

    // sends all data to thew new server
    sendProfileInfo(server);

    // sends other servers a message indicating a new backup server
    // sends new server messages with all previous servers
    for (Server *otherServer : _backupServers) {
        server->addMsg(Message(Socket::NEW_SERVER, std::to_string(otherServer->getID()) + " " + otherServer->getName() + " " + std::to_string(otherServer->getPort())));
        otherServer->addMsg(Message(Socket::NEW_SERVER, std::to_string(id) + " " + name + " " + std::to_string(portServer)));
    }

    _backupServers.push_back(server);

    _backupServersMutex.unlock();
    // starts threads
    startBackupThreads(addr, port, id, server);
}

// starts all servers from a backup server (elected main server)
void startServerFromBackup(int port) {
    _backupServersMutex.lock();

    
    for(Server *server: _backupServers){
        startBackupThreads(Socket::create_addr((char *)server->getName().c_str(), server->getPort()), port, server->getID(), server);    
    }

    _backupServersMutex.unlock();
}

// send thread
void sendBackupThread(std::shared_ptr<BackupConnection> sess) {
    sess->send();
}

// listen thread
void listenBackupThread(std::shared_ptr<BackupConnection> sess) {
    sess->listen();
}

// starts threads for main server to deal with backup server
void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server) {
    std::shared_ptr<BackupConnection> sess = std::make_shared<BackupConnection>(port, addr, id, server);
    std::thread listen_thread = std::thread(listenBackupThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendBackupThread, sess);
    send_thread.detach();
}