#include <iostream>
#include <string>
#include "../../include/server/profilemanager.hpp"


const std::string profileFile = "data/users.txt";


int main(int argc, char*argv[]) {

    if (argc < 2) {
        std::cerr << " ./app_server <porta>" << std::endl;
        exit(1);
    }

    int port = atoi(argv[1]);

    ProfileManager profileManager(profileFile);

    // TODO: connection manager thread
}