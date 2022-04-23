#include "../../include/server/backupthreads.hpp"
#include "../../include/server/servermanager.hpp"
#include<iostream>


bool _isMainServerAlive;
bool _isMainServerAliveSent;
bool _sessionClosed;

std::mutex _serverSessionMutex;
std::mutex _mainServerAliveMutex;
std::mutex _mainServerAliveSentMutex;

std::mutex msgs_mtx; // mutex for update_msgs queue
std::condition_variable msgs_cv; // condition variable that indicates that update_msgs is not empty

std::queue<Message> msgs; // queue for update messages


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

void createServerSendThread(std::shared_ptr<Socket> sock){
    while(1){
        // If the session was closed, ends the send
        if (serverSessionClosed()) break;


        std::unique_lock<std::mutex> lck(msgs_mtx);
        if (msgs_cv.wait_for(lck, std::chrono::microseconds(2), []{return !msgs.empty();}) == false)
            continue;        
        Message notification = msgs.front();
        msgs.pop();
        // If it is empty, keep waiting for new messages
        if(notification.getType() == "")
            continue;
        if(notification.getType() == Socket::ALIVE)
            setMainServerAliveSent(true);
        // Sends notification read to the client
        sock->send(Socket::SERVER_ACK + " " + notification.getType() + " " + notification.getArgs());
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

void createServerListenThread(std::shared_ptr<Socket> sock){
    std::thread listen_thread = std::thread(serverListenThread, sock);
    listen_thread.detach();
}

void serverListenThread(std::shared_ptr<Socket> sock){
    while(1){
        // listen for valid client messages
        std::string message = sock->listen();
        if (message == "")
            continue;

        std::vector<std::string> spMessage = sock->splitUpToMessage(message, 2);
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
    std::shared_ptr<Socket> sock = std::make_shared<Socket> (port);

    sock->setoth_addr(name, port_main);
    sock->send(Socket::CONNECT_SERVER);

    // Listen to server
    std::string result = sock->listen();
    std::string type = sock->getTypeMessage(result);

    // In case socket doesn't listen to a connection working, exit with error
    // If the connection is working, process start message
    if (type == Socket::CONNECT_NOT_OK){
        std::cout << "ERROR " << result << std::endl;
        exit(1);
    } else if(type == Socket::CONNECT_OK){
        std::vector<std::string> spMessage = sock->splitUpToMessage(result, 2);
        if(spMessage.size() < 2){
            std::cout << "ERROR " << result << std::endl;
            exit(1);
        }
        std::string idString = sock->splitUpToMessage(result, 2)[1];
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

    sock->closeSocket();
}