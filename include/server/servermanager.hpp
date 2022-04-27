#pragma once

#include <string>
#include <vector>
#include <mutex>
#include<memory>
#include "server.hpp"
#include "backupconnection.hpp"


void addCounterToMap(int id, std::shared_ptr<Counter> count);

std::shared_ptr<Counter> getCounterFromMap(int id);

void removeCounterFromMap(int id);

int getGlobalMessageCount();

void setGlobalMessageCount(int count);

void setSecPort(int port);

int getSecPort();

void setTercPort(int port);

int getTercPort();

void createServerManager(bool isPrimary);

void setServerIDAndCounter(int val);

int getServerID();

void printServers();

void addMessagetoServers(Message msg);

void addServer(int id, const std::string &name, int port);

void addBackupServer(int port, struct sockaddr_in addr);

void startServerFromBackup(int port);

std::vector<Server *> getBackupServers();

int getNumberServers();

void sendBackupThread(std::shared_ptr<BackupConnection> sess);

void listenBackupThread(std::shared_ptr<BackupConnection> sess);

void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server);

void removeFromBackupServers(int id);