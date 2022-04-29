#pragma once

#include "../common/socket.hpp"
#include <mutex>
#include <memory>

// setter/getter for MainServerAlive
void setMainServerAlive(bool newVal);
bool getMainServerAlive();

// setter/getter for MainServerAliveSent
void setMainServerAliveSent(bool newVal);
bool getMainServerAliveSent();

// adds alive message to queue to send to main server
void addAlivetoMainServerQueue();

// adds with id ack message to queue to send to main server
void addServerAcktoMainServerQueue(const std::string &id);

// checks if session is closed
bool serverSessionClosed();

// closes session
void closeServerSession();

// creates send thread
void createServerSendThread(std::shared_ptr<Socket> sock);

// alive thread, sends alive messages to main server
void AliveThread();
void createServerAliveThread();

// listen thread
void createServerListenThread(std::shared_ptr<Socket> sock);
void serverListenThread(std::shared_ptr<Socket> sock);

// creates connection to main server
void createConnectionToMainServer(char *name, int port, int port_main);

// starts election
void startElection(std::shared_ptr<Socket> sock);