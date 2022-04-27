#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/server/servermanager.hpp"
#include "../../include/server/backupthreads.hpp"
#include "../../include/common/socket.hpp"
#include <memory>
#include <chrono>

// This file contains all users saved on the server, that exist and logged in the server at some point
const std::string profileFile = "users.txt";

// Runs the the session manager's send method. (Required to create a send thread correctly)
void sendThread(std::shared_ptr<SessionManager> sess) {
    sess->send();
}

// Runs the the session manager's listen method. (Required to create a listen thread correctly)
void listenThread(std::shared_ptr<SessionManager> sess) {
    sess->listen();
}

// Creates threads for a new open session.
void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof) {
    // Creates a new session manager
    std::shared_ptr<SessionManager> sess = std::make_shared<SessionManager>(port_sec, addr, _prof);
    // Creates the listen and send threads to the new open session
    std::thread listen_thread = std::thread(listenThread, sess);
    listen_thread.detach();
    std::thread send_thread = std::thread(sendThread, sess);
    send_thread.detach();
}

int main(int argc, char *argv[]) {
    // Server arguments: connection port to listen for new sessions and ports to handle open sessions messages
    if (argc < 4) {
        std::cerr << "If primary server:" << std::endl;
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria> <porta_terciaria>" << std::endl;
        std::cerr << "If backup server:" << std::endl;
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria> <porta_terciaria> <porta_quaternaria> <primary_name> <port_main>" << std::endl;
        exit(1);
    }

    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);
    int port_ter = atoi(argv[3]);

    setTercPort(port_ter);

    // Gets users from the save file
    createProfileManager(profileFile);
    printProfiles();

    if (argc == 7) {
        int port_main = atoi(argv[6]);
        int port_quart = atoi(argv[4]);
        createServerManager(false);
        createConnectionToMainServer(argv[5], port_quart, port_main);
    } else if(argc == 4){
        createServerManager(true);
    } else {
        std::cerr << "If primary server:" << std::endl;
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria> <porta_terciaria>" << std::endl;
        std::cerr << "If backup server:" << std::endl;
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria> <porta_terciaria> <porta_quaternaria> <primary_name> <port_main>" << std::endl;
        exit(1);
    }

    // Opens connection through socket
    Socket sock(port_prim);
    while (1) {
        // listen for valid client messages
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if (spMessage.size() < 2)
            continue;

        // Check the type of the message
        std::string type = spMessage[0];
        if (type == Socket::CONNECT) {
            std::string name = spMessage[1];

            // Checks if the client profile exists, and if not, creates one
            Profile *prof = getProfile(name);
            if (prof == nullptr) {
                createProfile(name);
                prof = getProfile(name);
                startThreads(port_sec, sock.getoth_addr(), prof);
            } else if (prof->getSessions() < prof->MAX_SESSIONS) {
                startThreads(port_sec, sock.getoth_addr(), prof);

                // Prevents clients from having more than two active sessions
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
            // If it is not a valid type of message, returns an error
        } else if (type == Socket::CONNECT_SERVER) {
            addBackupServer(port_ter, sock.getoth_addr());
        } else {
            std::cout << "ERROR MAIN THREAD" << message << std::endl;
        }
    }
}