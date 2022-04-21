#pragma once

#include<string>
#include<vector>
#include<mutex>
#include"server.hpp"
#include"backupconnection.hpp"


void createServerManager(bool isPrimary);

void setServerIDAndCounter(int val);

void printServers();

void addBackupServer(int port, struct sockaddr_in addr);

void sendBackupThread(std::shared_ptr<BackupConnection> sess);

void listenBackupThread(std::shared_ptr<BackupConnection> sess);

void startBackupThreads(struct sockaddr_in addr, int port, int id, Server server);