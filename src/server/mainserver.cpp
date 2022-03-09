#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/common/socket.hpp"


const std::string profileFile = "data/users.txt";

void listenSec(Socket *sock){
    std::cout << "start thread listen: " <<std::endl;

    while(1){
        std::string message = sock->listen();
        std::vector<std::string> spMessage = sock->splitUpToMessage(message, 3);
        std::string type = spMessage[0];

        if (type == Socket::EXIT){
            std::cout << " EXIT" << std::endl;
            std::string prof = spMessage[1];
            std::string sess = spMessage[2];

            if(getSession(sess)){
                getSession(sess)->closeSession();
                getProfile(prof)->decrementSessions();
            }
        } else if (type == Socket::FOLLOW){
            std::cout <<  " FOLLOW" << std::endl;

            std::string prof = spMessage[1];
            std::string foll = spMessage[2];

            Profile *folProf = getProfile(foll);
            if (folProf == nullptr){
                std::cout << "Profile doesn't exist" << std::endl;
            } else {
                folProf->addFollower(prof, true);
            }
        } else if (type == Socket::SEND_NOTIFICATION){
            std::cout << " SEND_NOTIFICATION" << std::endl;

            std::string prof = spMessage[1];
            std::string msg = spMessage[2];

            getProfile(prof)->notifyFollowers(msg);
        } else {
            std::cout << " ERROR " + message << std::endl;
        }
    }
    std::cout << "end thread listen: "<< std::endl;
}


int main(int argc, char*argv[]) {

    if (argc < 3) {
        std::cerr << " ./app_server <porta_primaria> <porta_secundaria>" << std::endl;
        exit(1);
    }
    
    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);

    

    createProfileManager(profileFile);
    printProfiles();

    Socket sock_sec(port_sec);
    std::thread listen_thread = std::thread(listenSec, &sock_sec);
    listen_thread.detach();

    Socket sock(port_prim);
    while(1){
        std::string message = sock.listen();
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        std::string type = spMessage[0];

        if(type == Socket::CONNECT){
            std::string name = spMessage[1];
            Profile *prof = getProfile(name);
            if(prof == nullptr){
                createProfile(name);
                prof = getProfile(name);
                SessionManager sess(&sock_sec, sock.getoth_addr(), prof);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                SessionManager sess(&sock_sec, sock.getoth_addr(), prof);
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}