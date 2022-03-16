#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/common/socket.hpp"
#include <memory>

// This file contains all users saved on the server, that exists and logged in the server
const std::string profileFile = "users.txt";

// Runs the send of the server session manager
void sendThread(std::shared_ptr<SessionManager> sess){
    sess->send();
}

// Runs the listen of the server session manager
void listenThread(std::shared_ptr<SessionManager> sess){
    sess->listen();
}

// Runs all the server threads
void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof){
    // Creates the session manager
    std::shared_ptr<SessionManager> sess = std::make_shared<SessionManager>(port_sec, addr, _prof);
    // Creates the listen and send threads
    std::thread listen_thread = std::thread(listenThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendThread, sess);
    send_thread.detach();
}

int main(int argc, char*argv[]) {
    // Receiver start client arguments: profile, connection port for clients and connection port for the server
    if (argc < 3) {
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria>" << std::endl;
        exit(1);
    }   
    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);

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

            // Check if the client profile exists, and if not, creates one
            Profile *prof = getProfile(name);
            if(prof == nullptr){
                createProfile(name);
                prof = getProfile(name);
                startThreads(port_sec, sock.getoth_addr(), prof);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                startThreads(port_sec, sock.getoth_addr(), prof);

            // Prevents the client to have more than two active sessions 
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        // If it is not a valid type of message, returns an error
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}