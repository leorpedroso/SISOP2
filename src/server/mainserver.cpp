#include <iostream>
#include <string>
#include <vector>
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/sessionmanager.hpp"
#include "../../include/common/socket.hpp"


const std::string profileFile = "data/users.txt";


int main(int argc, char*argv[]) {

    if (argc < 3) {
        std::cerr << " ./app_server <porta_primaria> <porta_secundaria>" << std::endl;
        exit(1);
    }
    
    int port_prim = atoi(argv[1]);
    int port_sec = atoi(argv[2]);

    

    ProfileManager profileManager(profileFile);

    // TODO: connection manager thread
    Socket sock(port_prim);
    while(1){
        std::string message = sock.listen();
        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        std::string type = spMessage[0];

        if(type == sock.CONNECT){
            std::string name = spMessage[1];
            std::shared_ptr<Profile> prof = profileManager.getProfile(name);
            if(prof == nullptr){
                profileManager.createProfile(name);
                prof = profileManager.getProfile(name);
                SessionManager sess(port_sec, sock.getoth_addr(), *prof, profileManager);
            } else if (prof->getSessions() < prof->MAX_SESSIONS){
                SessionManager sess(port_sec, sock.getoth_addr(), *prof, profileManager);
            } else {
                sock.send(sock.CONNECT_NOT_OK + " Profile already has 2 Sessions");
            }
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}