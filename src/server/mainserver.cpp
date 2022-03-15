#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/common/socket.hpp"
#include <memory>


const std::string profileFile = "users.txt";

void sendThread(std::shared_ptr<SessionManager> sess){
    sess->send();
}

void listenThread(std::shared_ptr<SessionManager> sess){
    sess->listen();
}

void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof){
    std::shared_ptr<SessionManager> sess = std::make_shared<SessionManager>(port_sec, addr, _prof);

    std::thread listen_thread = std::thread(listenThread, sess);
    listen_thread.detach();

    std::thread send_thread = std::thread(sendThread, sess);
    send_thread.detach();
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

        // listen for messages
        std::string message = sock.listen();
        if (message == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if(spMessage.size() < 2)
                continue;

        std::string type = spMessage[0];

        // check for type
        if(type == Socket::CONNECT){
            std::string name = spMessage[1];

            // check if profile exists
            Profile *prof = getProfile(name);
            if(prof == nullptr){
                createProfile(name);
                prof = getProfile(name);
                startThreads(port_sec, sock.getoth_addr(), prof);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                startThreads(port_sec, sock.getoth_addr(), prof);
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}