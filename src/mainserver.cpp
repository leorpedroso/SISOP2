#include <iostream>
#include <string>
#include "../include/profilemanager.hpp"
#include <pthread.h>


pthread_t profileThread_t;
pthread_t connectionThread_t;
const std::string profileFile = "server/users.txt";


void *connectionThread(void *arg){
    // TODO connection manager thread
    //ConnectionManager manager = *((ConnectionManager *) arg);
    //manager.listen();
    //pthread_exit(NULL);
}

int main(int argc, char*argv[]) {

    if (argc < 2) {
        std::cerr << " ./app_server <porta>" << std::endl;
        exit(1);
    }

    int port = atoi(argv[1]);

    ProfileManager profileManager(profileFile);

    // TODO: connection manager thread
    // ConnectionManager connectionManager(port, profileManager);
    // pthread_create(&connectionThread_t, NULL, connectionThread, &connectionManager);
    // pthread_join(connectionThread_t, NULL);
}