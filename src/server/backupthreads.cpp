#include "../../include/server/backupthreads.hpp"
#include "../../include/server/servermanager.hpp"
#include <iostream>
#include<sstream>
#include<cstring>

// indicates if main server is alive
bool _isMainServerAlive;
std::mutex _mainServerAliveMutex;

// indicates if alive message was sent to main server
bool _isMainServerAliveSent;
std::mutex _mainServerAliveSentMutex;

// indicates if session is closed
bool _sessionClosed;
std::mutex _serverSessionMutex;

// indicates if current server is coordinator
bool _isCoordinator;
std::mutex _isCoordinatorMutex;

// indicates if election is over
bool _isElectionOver;
std::mutex _isElectionOverMutex;

// main server sockaddr_in
struct sockaddr_in _MainServer;
std::mutex _MainServerMutex;

// client sessions
std::unordered_set<std::string> _backupClientSessions;
std::mutex _backupClientSessionsMutex;

// queue for messages
std::queue<Message> _msgs;
std::mutex _msgs_mtx;             // mutex for _msgs queue
std::condition_variable _msgs_cv; // condition variable that indicates that _msgs is not empty

// start threads from main
void startThreads(int port_sec, struct sockaddr_in addr, Profile *_prof);

// getter/setter for main server sockaddr_in
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

// getter/add/remove client sessions
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

// setter/getter for _isElectionOver
void setElectionOver(bool newVal) {
    std::unique_lock<std::mutex> mlock(_isElectionOverMutex);
    _isElectionOver = newVal;
}

bool isElectionOver() {
    std::unique_lock<std::mutex> mlock(_isElectionOverMutex);
    return _isElectionOver;
}

// setter/getter for _isCoordinator
void setCoordinator(bool newVal) {
    std::unique_lock<std::mutex> mlock(_isCoordinatorMutex);
    _isCoordinator = newVal;
}
bool isCoordinator() {
    std::unique_lock<std::mutex> mlock(_isCoordinatorMutex);
    return _isCoordinator;
}

// setter/getter for _isMainServerAlive
void setMainServerAlive(bool newVal) {
    _mainServerAliveMutex.lock();

    _isMainServerAlive = newVal;

    _mainServerAliveMutex.unlock();
}
bool getMainServerAlive() {
    _mainServerAliveMutex.lock();

    bool temp = _isMainServerAlive;

    _mainServerAliveMutex.unlock();

    return temp;
}

// setter/getter for _isMainServerAliveSent
void setMainServerAliveSent(bool newVal) {
    _mainServerAliveSentMutex.lock();

    _isMainServerAliveSent = newVal;

    _mainServerAliveSentMutex.unlock();
}
bool getMainServerAliveSent() {
    _mainServerAliveSentMutex.lock();

    bool temp = _isMainServerAliveSent;

    _mainServerAliveSentMutex.unlock();

    return temp;
}

// adds alive message to msgs queue
void addAlivetoMainServerQueue() {
    std::unique_lock<std::mutex> lck(_msgs_mtx);
    _msgs.push(Message(Socket::ALIVE, "Alive"));
    _msgs_cv.notify_all();
}

// adds ack message to msgs queue
void addServerAcktoMainServerQueue(const std::string &id) {
    std::unique_lock<std::mutex> lck(_msgs_mtx);
    _msgs.push(Message(id, ""));
    _msgs_cv.notify_all();
}

// checks if session is closed
bool serverSessionClosed() {
    _serverSessionMutex.lock();

    bool temp = _sessionClosed;

    _serverSessionMutex.unlock();

    return temp;
}

// closes session
void closeServerSession() {
    _serverSessionMutex.lock();

    _sessionClosed = true;

    _serverSessionMutex.unlock();
}

// opens server session
void openServerSession() {
    _serverSessionMutex.lock();

    _sessionClosed = false;

    _serverSessionMutex.unlock();
}

// creates send thread
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
        // Sends notification read to the main server
        if (notification.getType() == Socket::ALIVE) {
            sock->send(Socket::ALIVE + " " + notification.getType() + " " + notification.getArgs(), getMainServer());
            setMainServerAliveSent(true);
        } else {
            sock->send(Socket::SERVER_ACK + " " + notification.getType() + " " + notification.getArgs(), getMainServer());
        }
    }
    std::cout << "end thread send main: " << send_id << std::endl;
}

// Alive thread
void createServerAliveThread() {
    std::thread alive_thread = std::thread(AliveThread);
    alive_thread.detach();
}
void AliveThread() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string alive_id = ss.str();
    std::cout << "start thread alive main: " << alive_id << std::endl;

    // considers main server alive
    setMainServerAlive(true);
    setMainServerAliveSent(false);


    while (1) {
        // waits to verify if main server is currently alive
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        if (getMainServerAlive()) {
            // if server is alive resets variables and adds alive to queue 
            setMainServerAlive(false);
            setMainServerAliveSent(false);
            addAlivetoMainServerQueue();
        } else if (getMainServerAliveSent()) {
            // alive was sent but a response wasn't received -> start election
            break;
        }
    }
    closeServerSession();

    std::cout << "end thread alive main: " << alive_id << std::endl;
}

// listen thread
void createServerListenThread(std::shared_ptr<Socket> sock) {
    std::thread listen_thread = std::thread(serverListenThread, sock);
    listen_thread.detach();
}
void serverListenThread(std::shared_ptr<Socket> sock) {
    // thread id
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string listen_id = ss.str();
    std::cout << "start thread listen main: " << listen_id << std::endl;

    while (1) {
        // if election is over and is coordinator, can close the thread
        if(isCoordinator() && isElectionOver())
            break;

        // listens to message, if empty skip
        std::string message = sock->listen();
        if (message == "")
            continue;

        // splits message and checks if split was successful
        std::vector<std::string> spMessage = sock->splitUpToMessage(message, 2);
        if (spMessage.size() < 2)
            continue;

        // Check the type of the message
        std::string type = spMessage[0];
        if (type == Socket::SERVER_UPDATE) {
            // server update message
            spMessage = Socket::splitUpToMessage(spMessage[1], 2);
            if (spMessage.size() < 2)
                continue;

            type = spMessage[0];

            if (type == Socket::NEW_SERVER) {
                // New server connected to main server
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                if (spMessage.size() < 3)
                    continue;

                // add server
                int id = stoi(spMessage[0]);
                std::string name = spMessage[1];
                int port = stoi(spMessage[2]);
                addServer(id, name, port);
            } else if (type == Socket::ALIVE) {
                // ack from main server
                setMainServerAlive(true);
                setCoordinator(false);
            } else if (type == Socket::ACK){
                // Ack sent to client

                // id prof msg
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                if (spMessage.size() < 3)
                    continue;

                std::string id = spMessage[0];
                std::string prof = spMessage[1];
                std::string message = spMessage[2];
                Profile *profileProfile = getProfile(prof);
                // resyncs counter
                setGlobalMessageCount(stoi(id));

                // message -> textType data0 data1
                // data0(FOLLOW) -> 0 | 1 | 2
                // data1(FOLLOW) -> follower
                // data0(SEND) -> time
                // data1(SEND) -> string
                // data1(ADD_NOT) -> sender string
                spMessage = Socket::splitUpToMessage(spMessage[2], 3);
                if (spMessage.size() < 3)
                    continue;

                if(spMessage[0] == "FOLLOW"){
                    // Client followed another profile

                    // 0 and 2 mean that the operation wasn't sucessfull
                    if(spMessage[1] == "1"){
                        // follow profile
                        Profile *folProf = getProfile(spMessage[2]);
                        if (folProf != nullptr) {
                            folProf->addFollower(prof, false);
                        }
                    }

                } else if(spMessage[0] == "SEND"){
                    // Client sent a message

                    // TODO problem with Counter?
                    // add message to all followers
                    if(profileProfile != nullptr){
                        profileProfile->notifyFollowers(spMessage[2], spMessage[1]);
                    }
                } else if(spMessage[0] == "ADD_NOT"){
                    // Pending notification
                    std::string time = spMessage[1];
                    spMessage = Socket::splitUpToMessage(spMessage[2], 2);
                    if (spMessage.size() < 2)
                        continue;

                    // adds notification to profile
                    if(profileProfile != nullptr){
                        profileProfile->putNotification(spMessage[1], spMessage[0], time);
                    }
                }
                
                // ack to main server
                addServerAcktoMainServerQueue(id);

            } else if(type == Socket::NOTIFICATION){
                // Notificaiton being sent to client
                spMessage = Socket::splitUpToMessage(spMessage[1], 3);
                if (spMessage.size() < 3)
                    continue;

                // resyncs counter
                setGlobalMessageCount(stoi(spMessage[0]));

                // TODO can the order be wrong?
                // pops the last notification because it is no longer pending
                Profile *profTemp = getProfile(spMessage[1]);
                if (profTemp != nullptr) {
                    profTemp->popNotification();
                }
                // ack to main server
                addServerAcktoMainServerQueue(spMessage[0]);

            } else if(type == Socket::CONNECT_OK){
                // client connected to main server

                // id prof addr port
                spMessage = Socket::splitUpToMessage(spMessage[1], 4);
                if (spMessage.size() < 4)
                    continue;

                // resyncs counter
                setGlobalMessageCount(stoi(spMessage[0]));

                // create profile if isn't created yet
                Profile *prof = getProfile(spMessage[1]);
                if (prof == nullptr) {
                    createProfile(spMessage[1], false);
                }
                // start session
                addBackupClientSession(spMessage[1], spMessage[2], spMessage[3]);
                // ack to main server
                addServerAcktoMainServerQueue(spMessage[0]);

            } else if(type == Socket::EXIT){
                // EXIT from a client

                // id prof addr port
                spMessage = Socket::splitUpToMessage(spMessage[1], 4);
                if (spMessage.size() < 4)
                    continue;

                // resyncs counter
                setGlobalMessageCount(stoi(spMessage[0]));

                // removes active session from client
                removeBackupClientSession(spMessage[1], spMessage[2], spMessage[3]);
                // ack to main server
                addServerAcktoMainServerQueue(spMessage[0]);
            } else {
                // ERROR
                std::cout << "ERROR " << message << std::endl;
            }

        } else if (type == Socket::ELECTION_START) {
            // Election was started, sends answer to the server
            for (Server *server : getBackupServers()) {
                if (server->getID() == stoi(spMessage[1])) {
                    sock->send(Socket::ELECTION_ANSWER + " ", 
                        Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
                }
            }
            // closes session to send ELECTION messages to other servers
            closeServerSession();
        } else if (type == Socket::ELECTION_ANSWER) {
            // if received an ANSWER it isn't the coordiantor
            setCoordinator(false);
        } else if (type == Socket::ELECTION_COORDINATOR) {
            // new coordiantor message
            spMessage = Socket::splitUpToMessage(spMessage[1], 2);
            if (spMessage.size() < 2)
                continue;
            int coordId = stoi(spMessage[0]);
            int coordPort = stoi(spMessage[1]);

            // set coordinator and new main server
            setCoordinator(false);
            setMainServer(sock->getoth_addr(), coordPort);
            removeFromBackupServers(coordId);
        } else if (type == Socket::CONNECT_SERVER_OK){
            //doesn't need to do anything 
        } else {
            // ERROR
            std::cout << "ERROR " << message << std::endl;
        }
    }
    // can close socket because listen will be the last thread alive that accesses the socket
    sock->closeSocket();
    std::cout << "end thread listen main: " << listen_id << std::endl;
}

void createConnectionToMainServer(char *name, int port, int port_main) {
    // inits socket
    std::shared_ptr<Socket> sock = std::make_shared<Socket>(port, true);

    setCoordinator(false);
    setElectionOver(true);

    // Sends message to server requesting login
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
        std::string idString = spMessage[1];
        setServerIDAndCounter(stoi(idString));

        std::cout << "Starting backup" << std::endl;
    } else {
        std::cout << "ERROR " << result << std::endl;
        exit(1);
    }

    // sets main server
    setMainServer(sock->getoth_addr());

    // inits listen thread
    createServerListenThread(sock);

    while (1) {
        openServerSession();

        // inits alive, send threads
        createServerAliveThread();
        createServerSendThread(sock);

        // start election
        startElection(sock);

        if(isCoordinator()) {
            std::cout << "NEW COORDINATOR." << std::endl;
            break;
        }
    }

    // starts threads for clients
    for(auto sess: getBackupClientSessions()){
        std::vector<std::string> spMessage = sock->splitUpToMessage(sess, 3);
        std::string prof = spMessage[0];
        std::string addr = spMessage[1];
        int port = stoi(spMessage[2]);
        char *char_array = &addr[0];

        startThreads(getSecPort(), Socket::create_addr(char_array, port), getProfile(prof));
    }

    // start threads for backup servers
    startServerFromBackup(getTercPort());

    // saves current profiles
    saveProfiles();
}

// starts election
void startElection(std::shared_ptr<Socket> sock) {
    // sets election variable
    setElectionOver(false);
    // assumes it is coordinator
    setCoordinator(true);

    std::cout << "Election started." << std::endl;

    // tries to send the previous coordinator a message
    sock->send(Socket::ALIVE + " " + Socket::ALIVE + " " + "ALIVE", getMainServer());
    // waits for coordinator message
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // if isnt coordinator end election (received message from main server)
    if(!isCoordinator()){
        setElectionOver(true);
        return;
    }


    // sends other servers (with id > current id) an ELECTION message
    for (Server *server : getBackupServers()) {
        if (server->getID() > getServerID()) {
            sock->send(Socket::ELECTION_START + " " + std::to_string(getServerID()), 
                Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
        }
    }
    // waits
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // if no one responded, current server is coordinator
    // sends COORDINATOR Message to other servers
    if (isCoordinator()) {
        std::cout << "Elected new coordinator. Sending message to other backups." << std::endl;
        for (Server *server : getBackupServers()) {
            sock->send(Socket::ELECTION_COORDINATOR + " " + std::to_string(getServerID()) + " " + std::to_string(getTercPort()),
                Socket::create_addr((char *)server->getName().c_str(), server->getPort()));
        }
    }

    // ends election
    setElectionOver(true);
}