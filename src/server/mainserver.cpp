#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/common/socket.hpp"


const std::string profileFile = "data/users.txt";

void startSendThread(SessionManager *sess){
    std::thread send_thread = std::thread(&SessionManager::send, sess);

    send_thread.detach();
}

void startListenThread(SessionManager *sess){
    std::thread listen_thread = std::thread(&SessionManager::listen, sess);

    listen_thread.detach();
}


int main(int argc, char*argv[]) {

    if (argc < 3) {
        std::cerr << " " << argv[0] << " <porta_primaria> <porta_secundaria>" << std::endl;
        exit(1);
    }
    
    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);

    

    createProfileManager(profileFile);
    printProfiles();


    Socket sock(port_prim);
    while(1){
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        std::string type = spMessage[0];

        if(type == Socket::CONNECT){
            std::string name = spMessage[1];
            Profile *prof = getProfile(name);
            if(prof == nullptr){
                createProfile(name);
                prof = getProfile(name);
                SessionManager *sess(new SessionManager (port_sec, sock.getoth_addr(), prof));
                startSendThread(sess);
                startListenThread(sess);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                SessionManager *sess(new SessionManager (port_sec, sock.getoth_addr(), prof));
                startSendThread(sess);
                startListenThread(sess);
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}