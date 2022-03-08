#include <iostream>
#include <string>
#include "../../include/client/interface.hpp"
#include "../../include/client/notificationmanagerclient.hpp"
#include <pthread.h>
#include <signal.h>
#include "../../include/common/socket.hpp"


pthread_t interfaceThread_t;
pthread_t notificationThread_t;

Socket sock;
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
    // Send exit message.
    sock.send(sock.EXIT);

    pthread_cancel(interfaceThread_t);

    sock.closeSocket();
}

int main(int argc, char*argv[]) {

    if (argc < 4) {
        std::cerr << " ./app_cliente <perfil> <endereço do servidor> <porta>" << std::endl;
        exit(1);
    }

    signal(SIGINT, signalHandler);

    std::string profile = argv[1];
    ClientArgs clientArgs(argv[1], argv[2], std::stoi(argv[3]));

    // 1. Send message to server requesting login
    sock.setoth_addr(argv[2], std::stoi(argv[3]));
    sock.send(sock.CONNECT + " " + clientArgs.profile);

    // 2. if result==OK continue else exit(1)
    std::string result = sock.listen();
    std::string type = sock.getTypeMessage(result);
    if (type == sock.CONNECT_NOT_OK){
        std::cout << "ERROR " << result << std::endl;
        exit(1);
    } else if(type == sock.CONNECT_OK){
        std::cout << "OK " << result << std::endl;
    } else {
        std::cout << "ERROR " << result << std::endl;
        exit(1);
    }

    Interface interface(clientArgs.profile, sock);
    NotificationManager manager(clientArgs.profile, sock, interface);

    pthread_create(&interfaceThread_t, NULL, interfaceThread, &interface);
    pthread_create(&notificationThread_t, NULL, notificationThread, &manager);

    pthread_join(interfaceThread_t, NULL);
    pthread_cancel(notificationThread_t);

    sock.closeSocket();
}