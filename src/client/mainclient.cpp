#include <iostream>
#include <string>
#include "../../include/client/interface.hpp"
#include "../../include/client/notificationmanagerclient.hpp"
#include <pthread.h>
#include <signal.h>
#include<mutex>
#include "../../include/common/socket.hpp"

// Threads to manage client interface and listen to notifications
pthread_t interfaceThread_t;
pthread_t notificationThread_t;

// Save profile name and number of access to ensure a user doesn't have more than two active sessions
std::string id;
std::string profile;

Socket sock;

void setAddr(struct sockaddr_in newAddr);
struct sockaddr_in getAddr();


std::mutex _addrMutex;
struct sockaddr_in _addr;

void setAddr(struct sockaddr_in newAddr){
    std::unique_lock<std::mutex> mlock(_addrMutex);
    _addr = newAddr;
}

struct sockaddr_in getAddr(){
    std::unique_lock<std::mutex> mlock(_addrMutex);
    return _addr;
}

// Struct to save the initial client arguments: profile, ip and connection port
struct ClientArgs{
    std::string profile;
    std::string ip;
    int port;

    ClientArgs(std::string profile, std::string ip, int port) : profile(profile), ip(ip), port(port){}
};

// Handle the listen method to receive messages from the server
void *notificationThread(void *arg) {
    NotificationManager manager = *((NotificationManager *) arg);
    manager.listen();
    pthread_exit(NULL);
}

// Handle the interface thread to treat output and input messages
void *interfaceThread(void *arg){
    Interface interface = *((Interface *) arg);
    interface.run();
    pthread_exit(NULL);
}

// Send exit message, close socket and kill interface thread
void signalHandler(int s){
    sock.send(sock.EXIT + " " + profile + " " + id, getAddr());
    pthread_cancel(interfaceThread_t);
    sock.closeSocket();
}

int main(int argc, char*argv[]) {

    // Receive initial client arguments: profile, server address and connection port
    if (argc < 4) {
        std::cerr << " " << argv[0] << " <perfil> <endereço do servidor> <porta>" << std::endl;
        exit(1);
    }
    sock.disableLog();
    signal(SIGINT, signalHandler);

    profile = argv[1];
    ClientArgs clientArgs(argv[1], argv[2], std::stoi(argv[3]));

    // Checks if profile name (@) has the correct length
    if(!(profile.size() >= 4 && profile.size() <= 20)){
        std::cout << "Profile name needs to be between 4 and 20 chars" << std::endl;
        exit(1);
    }

    // Sends message to server requesting login
    sock.setoth_addr(argv[2], std::stoi(argv[3]));
    setAddr(sock.create_addr(argv[2], std::stoi(argv[3])));
    sock.send(sock.CONNECT + " " + clientArgs.profile, getAddr());


    while(1){
        // Listen to server
        std::string result = sock.listen();

        if (result=="")
            continue;

        std::string type = sock.getTypeMessage(result);

        // In case socket doesn't listen to a connection working, exit with error
        // If the connection is working, process start message
        if (type == sock.CONNECT_NOT_OK){
            std::cout << "ERROR " << result << std::endl;
            exit(1);
        } else if(type == sock.CONNECT_OK){
            std::vector<std::string> spMessage = sock.splitUpToMessage(result, 2);
            if(spMessage.size() < 2){
                std::cout << "ERROR " << result << std::endl;
                exit(1);
            }
            setAddr(sock.getoth_addr());
            id = sock.splitUpToMessage(result, 2)[1];
            std::cout << "Starting client" << std::endl;
            break;
        } else {
            std::cout << "ERROR " << result << std::endl;
            exit(1);
        }
    }

    // Creates interface and notification client threads
    Interface interface(clientArgs.profile, sock);
    NotificationManager manager(clientArgs.profile, sock, interface);
    pthread_create(&interfaceThread_t, NULL, interfaceThread, &interface);
    pthread_create(&notificationThread_t, NULL, notificationThread, &manager);

    // Kill threads
    pthread_join(interfaceThread_t, NULL);
    pthread_cancel(notificationThread_t);

    // Ends connection
    sock.closeSocket();
}