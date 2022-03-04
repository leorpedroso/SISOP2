#include <iostream>
#include <string>
#include "../../include/client/interface.hpp"
#include "../../include/client/notificationmanagerclient.hpp"
#include <pthread.h>
#include <signal.h>


pthread_t interfaceThread_t;
pthread_t notificationThread_t;
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

void signalHandler(int s){
    // TODO
    // 1. Send exit message.

    pthread_cancel(interfaceThread_t);
}

int main(int argc, char*argv[]) {

    if (argc < 4) {
        std::cerr << " ./app_cliente <perfil> <endereço do servidor> <porta>" << std::endl;
        exit(1);
    }

    signal(SIGINT, signalHandler);

    std::string profile = argv[1];
    ClientArgs clientArgs(argv[1], argv[2], std::stoi(argv[3]));


    // TODO 
    // 1. Send message to server requesting login
    // 2. if result==OK continue else exit(1)

    Interface interface(clientArgs.profile, clientArgs.ip, clientArgs.port);
    NotificationManager manager(clientArgs.profile, clientArgs.ip, clientArgs.port, interface);

    pthread_create(&interfaceThread_t, NULL, interfaceThread, &interface);
    pthread_create(&notificationThread_t, NULL, notificationThread, &manager);

    pthread_join(interfaceThread_t, NULL);
    pthread_cancel(notificationThread_t);
}