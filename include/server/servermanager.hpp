#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "server.hpp"
#include "backupconnection.hpp"


void setTercPort(int port);

int getTercPort();

void createServerManager(bool isPrimary);

void setServerIDAndCounter(int val);

int getServerID();

void printServers();

void addServer(int id, const std::string &name, int port);

void addBackupServer(int port, struct sockaddr_in addr);

void startServerFromBackup(int port);

std::vector<Server *> getBackupServers();

void sendBackupThread(std::shared_ptr<BackupConnection> sess);

void listenBackupThread(std::shared_ptr<BackupConnection> sess);

void startBackupThreads(struct sockaddr_in addr, int port, int id, Server *server);

void removeFromBackupServers(int id);