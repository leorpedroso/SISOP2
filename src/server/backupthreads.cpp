#include "../../include/server/backupthreads.hpp"
#include "../../include/server/servermanager.hpp"
#include<iostream>


bool _isMainServerAlive;
bool _isMainServerAliveSent;
bool _sessionClosed;

std::mutex _serverSessionMutex;
std::mutex _mainServerAliveMutex;
std::mutex _mainServerAliveSentMutex;


void setMainServerAlive(bool newVal){
    _mainServerAliveMutex.lock();

    _isMainServerAlive = newVal;

    _mainServerAliveMutex.unlock();
}

void setMainServerAliveSent(bool newVal){
    _mainServerAliveSentMutex.lock();

    _isMainServerAliveSent = newVal;

    _mainServerAliveSentMutex.unlock();
}

bool getMainServerAlive(){
    _mainServerAliveMutex.lock();

    bool temp = _isMainServerAlive;

    _mainServerAliveMutex.unlock();

    return temp;
}

bool getMainServerAliveSent(){
    _mainServerAliveSentMutex.lock();

     bool temp = _isMainServerAliveSent;

    _mainServerAliveSentMutex.unlock();

    return temp;
}

void addAlivetoMainServerQueue(){
    // TODO ADD MESSAGE TO QUEUE
}

bool serverSessionClosed(){
    _serverSessionMutex.lock();

    bool temp = _sessionClosed;

    _serverSessionMutex.unlock();

    return temp;
}

void closeServerSession(){
    _serverSessionMutex.lock();

    _sessionClosed = false;

    _serverSessionMutex.unlock();
}

void createServerSendThread(Socket sock){
    while(1){
        // If the session was closed, ends the send
        if (serverSessionClosed()) break;

        // Checks if there is a notification waiting to be sent to the client
        // TODO MESSAGE SEND
        /*Message notification = readMessageFromQueue();
        if(notification.getMessage() == "")
            continue;
        if(notification.getType() == Message::ALIVE){
            setMainServerAliveSent(true);
            sock.send(notification.getType());
        } else {
            sock.send(notification.getType() + " " + notification.getSender() + " " +notification.getMessage());
        }*/
    }
}

void AliveThread(){
    setMainServerAlive(true);
    setMainServerAliveSent(false);
    while(1){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if(getMainServerAlive()){
            setMainServerAlive(false);
            setMainServerAliveSent(false);
            addAlivetoMainServerQueue();
        } else if(getMainServerAliveSent()){
            break;
        }
    }
    closeServerSession();
}

void createServerAliveThread(){
    std::thread alive_thread = std::thread(AliveThread);
    alive_thread.detach();
}

void createServerListenThread(Socket sock){
    std::thread listen_thread = std::thread(serverListenThread, sock);
    listen_thread.detach();
}

void serverListenThread(Socket sock){
    while(1){
        // listen for valid client messages
        std::string message = sock.listen();
        if (message == "")
            continue;

        std::vector<std::string> spMessage = sock.splitUpToMessage(message, 2);
        if(spMessage.size() < 2)
            continue;

        // Check the type of the message  
        std::string type = spMessage[0];
        if(type == Socket::SERVER_UPDATE){
            // TODO SERVER UPDATES
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }
}

void createConnectionToMainServer(char *name, int port, int port_main){
    // Sends message to server requesting login
    Socket sock(port);

    sock.setoth_addr(name, port_main);
    sock.send(sock.CONNECT_SERVER);

    // Listen to server
    std::string result = sock.listen();
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
        std::string idString = sock.splitUpToMessage(result, 2)[1];
        setServerIDAndCounter(stoi(idString));

        std::cout << "Starting backup" << std::endl;
    } else {
        std::cout << "ERROR " << result << std::endl;
        exit(1);
    }

    createServerAliveThread();
    createServerListenThread(sock);
    createServerSendThread(sock);
    // TODO ELECTION

    sock.closeSocket();
}