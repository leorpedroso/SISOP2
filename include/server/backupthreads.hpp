#pragma once

#include "../common/socket.hpp"
#include <mutex>
#include <memory>

void setMainServerAlive(bool newVal);

void setMainServerAliveSent(bool newVal);

bool getMainServerAlive();

bool getMainServerAliveSent();

void addAlivetoMainServerQueue();

void addServerAcktoMainServerQueue(const std::string &id);

bool serverSessionClosed();

void closeServerSession();

void createServerSendThread(std::shared_ptr<Socket> sock);

void AliveThread();

void createServerAliveThread();

void createServerListenThread(std::shared_ptr<Socket> sock);

void serverListenThread(std::shared_ptr<Socket> sock);

void createConnectionToMainServer(char *name, int port, int port_main);

void startElection(std::shared_ptr<Socket> sock);