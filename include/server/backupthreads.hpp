#pragma once

#include "../common/socket.hpp"
#include<mutex>

void setMainServerAlive(bool newVal);

void setMainServerAliveSent(bool newVal);

bool getMainServerAlive();

bool getMainServerAliveSent();

void addAlivetoMainServerQueue();

bool serverSessionClosed();

void closeServerSession();

void createServerSendThread(Socket sock);

void AliveThread();

void createServerAliveThread();

void createServerListenThread(Socket sock);

void serverListenThread(Socket sock);

void createConnectionToMainServer(char *name, int port, int port_main);