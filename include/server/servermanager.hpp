#pragma once

#include<string>
#include<vector>
#include<mutex>
#include"server.hpp"

extern bool isServerPrimary;
extern unsigned int serverID;
extern unsigned int IDCounter;
extern std::vector<Server> backupServers;

// Functions for managing profiles

void createServerManager(bool isPrimary);

void printServers();

void addBackupServer(struct sockaddr_in addr, int port);