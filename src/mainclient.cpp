#include <iostream>
#include <string>
#include "../include/interface.hpp"
#include "../include/notificationmanagerclient.hpp"
#include <pthread.h>

struct ClientArgs{
    std::string profile;
    std::string ip;
    int port;

    ClientArgs(std::string profile, std::string ip, int port) : profile(profile), ip(ip), port(port){}
};

void *notificationThread(void *arg) {
    NotificationManager manager = *((NotificationManager *) arg);
    manager.listen();
    pthread_exit(NULL);
}

void *interfaceThread(void *arg){
    Interface interface = *((Interface *) arg);
    interface.run();
    pthread_exit(NULL);
}

int main(int argc, char*argv[]) {

    if (argc < 4) {
        std::cerr << " ./app_cliente <perfil> <endereço do servidor> <porta>" << std::endl;
        exit(1);
    }

    std::string profile = argv[1];
    ClientArgs clientArgs(argv[1], argv[2], std::stoi(argv[3]));


    // TODO 
    // 1. Send message to server requesting login
    // 2. if result==OK continue else exit(1)

    Interface interface(clientArgs.profile, clientArgs.ip, clientArgs.port);
    NotificationManager manager(clientArgs.profile, clientArgs.ip, clientArgs.port, interface);

    pthread_t interfaceThread_t;
    pthread_t notificationThread_t;

    pthread_create(&interfaceThread_t, NULL, interfaceThread, &interface);
    pthread_create(&notificationThread_t, NULL, notificationThread, &manager);

    pthread_join(interfaceThread_t, NULL);
    pthread_cancel(notificationThread_t);
}