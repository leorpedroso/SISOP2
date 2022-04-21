#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/server/servermanager.hpp"
#include "../../include/common/socket.hpp"
#include <memory>
#include <chrono>

// This file contains all users saved on the server, that exist and logged in the server at some point
const std::string profileFile = "users.txt";

// Runs the the session manager's send method. (Required to create a send thread correctly)
void sendThread(std::shared_ptr<SessionManager> sess){
    sess->send();
}

// Runs the the session manager's listen method. (Required to create a listen thread correctly)
void listenThread(std::shared_ptr<SessionManager> sess){
    sess->listen();
}

// Creates threads for a new open session.
void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof){
    // Creates a new session manager
    std::shared_ptr<SessionManager> sess = std::make_shared<SessionManager>(port_sec, addr, _prof);
    // Creates the listen and send threads to the new open session
    std::thread listen_thread = std::thread(listenThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendThread, sess);
    send_thread.detach();
}

void createServerSendThread(Socket sock, const std::string &nameMain, int port){
    while(1){
        // If the session was closed, ends the send
        if (serverSessionClosed()) break;

        // Checks if there is a notification waiting to be sent to the client
        Message notification = readMessageFromQueue();
        if(notification.getMessage() == "")
            continue;
        if(notification.getType() == Message::ALIVE){
            setMainServerAliveSent(true);
            sock.send(notification.getType());
        } else {
            sock.send(notification.getType() + " " + notification.getSender() + " " +notification.getMessage());
        }
    }
    // TODO ELECTION
}

void AliveThread(){
    setMainServerAlive(true);
    while(1){
        std::this_thread::sleep_for(2000ms);
        if(getMainServerAlive()){
            setMainServerAlive(false);
            setMainServerAliveSent(false);
            addAlivetoMainServerQueue();
        } else if(isMainServerAliveSent()){
            break;
        }
    }
    closeServerSession();
}

void createServerListenThread(Socket sock, int port){
    std::thread listen_thread = std::thread(serverListenThread, sock, port);
    listen_thread.detach();
}

void serverListenThread(Socket sock, int port){
    while(1){
        // listen for valid client messages
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if(spMessage.size() < 2)
                continue;

        // Check the type of the message  
        std::string type = spMessage[0];
        if(type == Socket::CONNECTSERVER){
            addBackupServer(sock.getoth_addr, port);
        } else if(type == Socket::SERVERUPDATE){
            // TODO
        } else if(type == Socket::ACKSERVER){
            // TODO
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}

int main(int argc, char*argv[]) {
    // Server arguments: connection port to listen for new sessions and port to handle open sessions messages
    if (argc < 5) {
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria> <porta_terciaria> <porta_quaternaria>" << std::endl;
        exit(1);
    }   
    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);
    int port_ter = atoi(argv[3]);
    int port_qua = atoi(argv[4]);

    Socket sockServer(port_ter);

    if (argc > 5){
        createServerManager(false);
        createServerListenThread(sockServer, port_qua);
        createServerSendThread(sockServer, argv[5], port_ter);
    } else {
        createServerManager(true);
        createServerListenThread(sockServer, port_qua);
    }

    // Gets users from the save file
    createProfileManager(profileFile);
    printProfiles();

    // Opens connection through socket
    Socket sock(port_prim);
    while(1){
        // listen for valid client messages
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if(spMessage.size() < 2)
                continue;

        // Check the type of the message  
        std::string type = spMessage[0];
        if(type == Socket::CONNECT){
            std::string name = spMessage[1];

            // Checks if the client profile exists, and if not, creates one
            Profile *prof = getProfile(name);
            if(prof == nullptr){
                createProfile(name);
                prof = getProfile(name);
                startThreads(port_sec, sock.getoth_addr(), prof);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                startThreads(port_sec, sock.getoth_addr(), prof);

            // Prevents clients from having more than two active sessions 
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        // If it is not a valid type of message, returns an error
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}