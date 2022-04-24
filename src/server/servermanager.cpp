#include "../../include/server/servermanager.hpp"

#include "../../include/server/backupconnection.hpp"
#include <iostream>
#include <thread>
#include <mutex>

bool _isServerPrimary;
int _serverID;
int _IDCounter;
std::vector<Server *> _backupServers;
std::mutex _backupServersMutex;
std::mutex _serverIDMutex;

void createServerManager(bool isPrimary) {
    _isServerPrimary = isPrimary;
    _serverID = 0;
}

void setServerIDAndCounter(int val) {
    std::unique_lock<std::mutex> mlock(_serverIDMutex);
    _serverID = val;
    _IDCounter = val;
}

int getServerID() {
    std::unique_lock<std::mutex> mlock(_serverIDMutex);
    return _serverID;
}

void printServers() {
    _backupServersMutex.lock();

    for (Server *server : _backupServers) {
        std::cout << "NAME: " << server->getName() << " ID: " << server->getID() << std::endl;
    }

    _backupServersMutex.unlock();
}

void addMessagetoServers(Message msg) {
    _backupServersMutex.lock();

    for (Server *server : _backupServers) {
        server->addMsg(msg);
    }

    _backupServersMutex.unlock();
}

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

void addServer(int id, const std::string &name, int port) {
    _backupServersMutex.lock();

    Server *server = new Server(id, name, port);

    _backupServers.push_back(server);

    _backupServersMutex.unlock();
}

std::vector<Server *> getBackupServers() {
    std::unique_lock<std::mutex> mlock(_backupServersMutex);
    return _backupServers;
}

void addBackupServer(int port, struct sockaddr_in addr) {
    _backupServersMutex.lock();

    Server *server = new Server(addr, ++_IDCounter);

    std::string name = server->getName();
    int portServer = server->getPort();
    int id = _IDCounter;

    for (Server *otherServer : _backupServers) {
        server->addMsg(Message(Socket::NEW_SERVER, std::to_string(otherServer->getID()) + " " + otherServer->getName() + " " + std::to_string(otherServer->getPort())));
        otherServer->addMsg(Message(Socket::NEW_SERVER, std::to_string(id) + " " + name + " " + std::to_string(portServer)));
    }

    _backupServers.push_back(server);

    _backupServersMutex.unlock();
    startBackupThreads(addr, port, id, server);
}

void sendBackupThread(std::shared_ptr<BackupConnection> sess) {
    sess->send();
}

// Runs the the session manager's listen method. (Required to create a listen thread correctly)
void listenBackupThread(std::shared_ptr<BackupConnection> sess) {
    sess->listen();
}

void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server) {
    std::shared_ptr<BackupConnection> sess = std::make_shared<BackupConnection>(port, addr, id, server);
    std::thread listen_thread = std::thread(listenBackupThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendBackupThread, sess);
    send_thread.detach();
}