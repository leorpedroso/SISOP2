#include "../../include/server/backupthreads.hpp"
#include "../../include/server/servermanager.hpp"
#include <iostream>
#include<sstream>
#include<cstring>

bool _isMainServerAlive;
bool _isMainServerAliveSent;
bool _sessionClosed;
bool _isCoordinator;
bool _isElectionOver;
struct sockaddr_in _MainServer;

std::mutex _serverSessionMutex;
std::mutex _mainServerAliveMutex;
std::mutex _mainServerAliveSentMutex;
std::mutex _isCoordinatorMutex;
std::mutex _isElectionOverMutex;
std::mutex _MainServerMutex;

std::unordered_set<std::string> _backupClientSessions;
std::mutex _backupClientSessionsMutex;

std::mutex _msgs_mtx;             // mutex for _msgs queue
std::condition_variable _msgs_cv; // condition variable that indicates that _msgs is not empty

std::queue<Message> _msgs; // queue for messages

void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof);

struct sockaddr_in getMainServer(){
    std::unique_lock<std::mutex> mlock(_MainServerMutex);

    return _MainServer;
}

void setMainServer(struct sockaddr_in newServer){
    std::unique_lock<std::mutex> mlock(_MainServerMutex);

    _MainServer = newServer;
}

void setMainServer(struct sockaddr_in newServer, int newPort){
    std::unique_lock<std::mutex> mlock(_MainServerMutex);

    _MainServer = newServer;
    _MainServer.sin_port = htons(newPort);
}

void addBackupClientSession(const std::string &prof, const std::string &addr, const std::string &port){
    std::unique_lock<std::mutex> mlock(_backupClientSessionsMutex);

    _backupClientSessions.insert(prof + " " + addr + " " + port);
}

void removeBackupClientSession(const std::string &prof, const std::string &addr, const std::string &port){
    std::unique_lock<std::mutex> mlock(_backupClientSessionsMutex);

    _backupClientSessions.erase(prof + " " + addr + " " + port);
}

std::unordered_set<std::string> getBackupClientSessions(){
    std::unique_lock<std::mutex> mlock(_backupClientSessionsMutex);

    return _backupClientSessions;
}


void setElectionOver(bool newVal) {
    std::unique_lock<std::mutex> mlock(_isElectionOverMutex);
    _isElectionOver = newVal;
}

bool isElectionOver() {
    std::unique_lock<std::mutex> mlock(_isElectionOverMutex);
    return _isElectionOver;
}

void setCoordinator(bool newVal) {
    std::unique_lock<std::mutex> mlock(_isCoordinatorMutex);
    _isCoordinator = newVal;
}

bool isCoordinator() {
    std::unique_lock<std::mutex> mlock(_isCoordinatorMutex);
    return _isCoordinator;
}

void setMainServerAlive(bool newVal) {
    _mainServerAliveMutex.lock();

    _isMainServerAlive = newVal;

    _mainServerAliveMutex.unlock();
}

void setMainServerAliveSent(bool newVal) {
    _mainServerAliveSentMutex.lock();

    _isMainServerAliveSent = newVal;

    _mainServerAliveSentMutex.unlock();
}

bool getMainServerAlive() {
    _mainServerAliveMutex.lock();

    bool temp = _isMainServerAlive;

    _mainServerAliveMutex.unlock();

    return temp;
}

bool getMainServerAliveSent() {
    _mainServerAliveSentMutex.lock();

    bool temp = _isMainServerAliveSent;

    _mainServerAliveSentMutex.unlock();

    return temp;
}

void addAlivetoMainServerQueue() {
    std::unique_lock<std::mutex> lck(_msgs_mtx);
    _msgs.push(Message(Socket::ALIVE, "Alive"));
    _msgs_cv.notify_all();
}

void addServerAcktoMainServerQueue(const std::string &id) {
    std::unique_lock<std::mutex> lck(_msgs_mtx);
    _msgs.push(Message(id, ""));
    _msgs_cv.notify_all();
}

bool serverSessionClosed() {
    _serverSessionMutex.lock();

    bool temp = _sessionClosed;

    _serverSessionMutex.unlock();

    return temp;
}

void closeServerSession() {
    _serverSessionMutex.lock();

    _sessionClosed = true;

    _serverSessionMutex.unlock();
}

void openServerSession() {
    _serverSessionMutex.lock();

    _sessionClosed = false;

    _serverSessionMutex.unlock();
}

void createServerSendThread(std::shared_ptr<Socket> sock) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string send_id = ss.str();
    std::cout << "start thread send main: " << send_id << std::endl;

    while (1) {
        // If the session was closed, ends the send
        if (serverSessionClosed())
            break;

        std::unique_lock<std::mutex> lck(_msgs_mtx);
        if (_msgs_cv.wait_for(lck, std::chrono::microseconds(2), [] { return !_msgs.empty(); }) == false)
            continue;
        Message notification = _msgs.front();
        _msgs.pop();
        // If it is empty, keep waiting for new messages
        if (notification.getType() == "")
            continue;
        if (notification.getType() == Socket::ALIVE) {
            sock->send(Socket::ALIVE + " " + notification.getType() + " " + notification.getArgs(), getMainServer());
            setMainServerAliveSent(true);
        } else {
            // Sends notification read to the main server
            sock->send(Socket::SERVER_ACK + " " + notification.getType() + " " + notification.getArgs(), getMainServer());
        }
    }
    std::cout << "end thread send main: " << send_id << std::endl;
}

void AliveThread() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string alive_id = ss.str();
    std::cout << "start thread alive main: " << alive_id << std::endl;

    setMainServerAlive(true);
    setMainServerAliveSent(false);


    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        if (getMainServerAlive()) {
            setMainServerAlive(false);
            setMainServerAliveSent(false);
            addAlivetoMainServerQueue();
        } else if (getMainServerAliveSent()) {
            break;
        }
    }
    closeServerSession();

    std::cout << "end thread alive main: " << alive_id << std::endl;
}

void createServerAliveThread() {
    std::thread alive_thread = std::thread(AliveThread);
    alive_thread.detach();
}

void createServerListenThread(std::shared_ptr<Socket> sock) {
    std::thread listen_thread = std::thread(serverListenThread, sock);
    listen_thread.detach();
}

void serverListenThread(std::shared_ptr<Socket> sock) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string listen_id = ss.str();
    std::cout << "start thread listen main: " << listen_id << std::endl;

    while (1) {
        if(isCoordinator() && isElectionOver())
            break;

        std::string message = sock->listen();
        if (message == "")
            continue;

        std::vector<std::string> spMessage = sock->splitUpToMessage(message, 2);
        if (spMessage.size() < 2)
            continue;

        // Check the type of the message
        std::string type = spMessage[0];
        if (type == Socket::SERVER_UPDATE) {
            spMessage = Socket::splitUpToMessage(spMessage[1], 2);
            type = spMessage[0];

            if (type == Socket::NEW_SERVER) {
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                int id = stoi(spMessage[0]);
                std::string name = spMessage[1];
                int port = stoi(spMessage[2]);

                addServer(id, name, port);
            } else if (type == Socket::ALIVE) {
                setMainServerAlive(true);
                setCoordinator(false);
            } else if (type == Socket::ACK){
                // id prof msg
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                std::string id = spMessage[0];
                std::string prof = spMessage[1];
                std::string message = spMessage[2];
                setGlobalMessageCount(stoi(id));

                // message -> textType data0 data1
                // data0(FOLLOW) -> 0 | 1 | 2
                // data1(FOLLOW) -> follower
                // data0(SEND) -> time
                // data1(SEND) -> string
                // data1(ADD_NOT) -> sender string
                spMessage = Socket::splitUpToMessage(spMessage[2], 3);

                if(spMessage[0] == "FOLLOW"){
                    // 0 and 2 mean that the operation wasn't sucessfull
                    if(spMessage[1] == "1"){
                        Profile *folProf = getProfile(spMessage[2]);
                        folProf->addFollower(prof, false);
                    }

                } else if(spMessage[0] == "SEND"){
                    // TODO problem with Counter?
                    getProfile(prof)->notifyFollowers(spMessage[2], spMessage[1]);
                } else if(spMessage[0] == "ADD_NOT"){
                    std::string time = spMessage[1];
                    spMessage = Socket::splitUpToMessage(spMessage[2], 2);
                    getProfile(prof)->putNotification(spMessage[1], spMessage[0], time);
                }

                addServerAcktoMainServerQueue(id);

            } else if(type == Socket::NOTIFICATION){
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                setGlobalMessageCount(stoi(spMessage[0]));

                // TODO can the order be wrong?
                getProfile(spMessage[1])->popNotification();

                addServerAcktoMainServerQueue(spMessage[0]);

            } else if(type == Socket::CONNECT_OK){
                // id prof addr port
                spMessage = Socket::splitUpToMessage(spMessage[1], 4);
                setGlobalMessageCount(stoi(spMessage[0]));

                Profile *prof = getProfile(spMessage[1]);
                if (prof == nullptr) {
                    createProfile(spMessage[1], false);
                }

                addBackupClientSession(spMessage[1], spMessage[2], spMessage[3]);

                addServerAcktoMainServerQueue(spMessage[0]);

            } else if(type == Socket::EXIT){
                // id prof addr port
                spMessage = Socket::splitUpToMessage(spMessage[1], 4);
                setGlobalMessageCount(stoi(spMessage[0]));

                removeBackupClientSession(spMessage[1], spMessage[2], spMessage[3]);

                addServerAcktoMainServerQueue(spMessage[0]);
            } else {
                std::cout << "ERROR " << message << std::endl;
            }

        } else if (type == Socket::ELECTION_START) {
            for (Server *server : getBackupServers()) {
                if (server->getID() == stoi(spMessage[1])) {
                    sock->send(Socket::ELECTION_ANSWER + " ", 
                        Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
                }
            }
            closeServerSession();
        } else if (type == Socket::ELECTION_ANSWER) {
            setCoordinator(false);
        } else if (type == Socket::ELECTION_COORDINATOR) {
            spMessage = Socket::splitUpToMessage(spMessage[1], 2);
            int coordId = stoi(spMessage[0]);
            int coordPort = stoi(spMessage[1]);

            setCoordinator(false);
            setMainServer(sock->getoth_addr(), coordPort);
            removeFromBackupServers(coordId);
        } else if (type == Socket::CONNECT_SERVER_OK){
            //setMainServer(sock->getoth_addr());
        } else {
            std::cout << "ERROR " << message << std::endl;
        }
    }

    std::cout << "end thread listen main: " << listen_id << std::endl;
}

void createConnectionToMainServer(char *name, int port, int port_main) {
    // Sends message to server requesting login
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(port, true);

    setCoordinator(false);
    setElectionOver(true);

    sock->setoth_addr(name, port_main);
    sock->send(Socket::CONNECT_SERVER + " " + std::to_string(port));

    // Listen to server
    std::string result = sock->listen();
    std::string type = sock->getTypeMessage(result);

    // In case socket doesn't listen to a connection working, exit with error
    // If the connection is working, process start message
    if (type == Socket::CONNECT_NOT_OK) {
        std::cerr << "ERROR " << result << std::endl;
        exit(1);
    } else if (type == Socket::CONNECT_SERVER_OK) {
        std::vector<std::string> spMessage = sock->splitUpToMessage(result, 2);
        if (spMessage.size() < 2) {
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

    setMainServer(sock->getoth_addr());

    createServerListenThread(sock);

    while (1) {
        openServerSession();

        createServerAliveThread();
        createServerSendThread(sock);

        startElection(sock);

        if(isCoordinator()) {
            std::cout << "NEW COORDINATOR." << std::endl;
            break;
        }
    }

    //sock->closeSocket();

    for(auto sess: getBackupClientSessions()){
        std::vector<std::string> spMessage = sock->splitUpToMessage(sess, 3);
        std::string prof = spMessage[0];
        std::string addr = spMessage[1];
        int port = stoi(spMessage[2]);
        char *char_array = &addr[0];

        startThreads(getSecPort(), Socket::create_addr(char_array, port), getProfile(prof));
    }

    startServerFromBackup(getTercPort());

    saveProfiles();
}

void startElection(std::shared_ptr<Socket> sock) {
    setElectionOver(false);
    setCoordinator(true);

    std::cout << "Election started." << std::endl;

    // tries to send the previous coordinator a message
    sock->send(Socket::ALIVE + " " + Socket::ALIVE + " " + "ALIVE", getMainServer());

    for (Server *server : getBackupServers()) {
        if (server->getID() > getServerID()) {
            sock->send(Socket::ELECTION_START + " " + std::to_string(getServerID()), 
                Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    if (isCoordinator()) {
        std::cout << "Elected new coordinator. Sending message to other backups." << std::endl;
        for (Server *server : getBackupServers()) {
            sock->send(Socket::ELECTION_COORDINATOR + " " + std::to_string(getServerID()) + " " + std::to_string(getTercPort()),
                Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
        }
    }

    setElectionOver(true);
}