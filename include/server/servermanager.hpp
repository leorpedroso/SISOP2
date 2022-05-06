#pragma once

#include <string>
#include <vector>
#include <mutex>
#include<memory>
#include "server.hpp"
#include "backupconnection.hpp"
#include "counter.hpp"

// mutex for messages that require order
extern std::mutex _messageOrderMutex;

// functions for adding counters to map, removing counters, getting counters,
void addCounterToMap(int id, std::shared_ptr<Counter> count);
std::shared_ptr<Counter> getCounterFromMap(int id);
void removeCounterFromMap(int id);

// getter/setter for GlobalMessageCount
int getGlobalMessageCount();
void setGlobalMessageCount(int count);

// getters/setters for ports
void setSecPort(int port);
int getSecPort();
void setTercPort(int port);
int getTercPort();

// creates server manager
void createServerManager();

// getter/setter for ID and counter
void setServerIDAndCounter(int val);
int getServerID();

// prints servers info
void printServers();

// adds message to all servers
void addMessagetoServers(Message msg, std::shared_ptr<Counter> count);

// adds new backup server to backup servers
void addServer(int id, const std::string &name, int port);
// addss new backup server to main server
void addBackupServer(int port, struct sockaddr_in addr);
// removes backup server
void removeFromBackupServers(int id);

// start server
void startServerFromBackup(int port);

// backup servers getter
std::vector<Server *> getBackupServers();
// num servers getter
int getNumberServers();

// main server send/listen thread for backups
void sendBackupThread(std::shared_ptr<BackupConnection> sess);
void listenBackupThread(std::shared_ptr<BackupConnection> sess);
void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server);