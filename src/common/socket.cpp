#include "../../include/common/socket.hpp"

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <netdb.h>
#include <sstream>
#include <strings.h>
#include <stdio.h>

// Command constants recognized by the client and server
const std::string Socket::CONNECT = "0";
const std::string Socket::CONNECT_OK = "1";
const std::string Socket::CONNECT_NOT_OK = "2";
const std::string Socket::EXIT = "3";
const std::string Socket::FOLLOW = "4";
const std::string Socket::SEND_NOTIFICATION = "5";
const std::string Socket::NOTIFICATION = "6";
const std::string Socket::ACK = "7";
const std::string Socket::CONNECT_SERVER = "8";
const std::string Socket::SERVER_UPDATE = "9";
const std::string Socket::ALIVE = "10";
const std::string Socket::SERVER_ACK = "11";
const std::string Socket::NEW_SERVER = "12";
const std::string Socket::ELECTION_START = "13";
const std::string Socket::ELECTION_ANSWER = "14";
const std::string Socket::ELECTION_COORDINATOR = "15";
const std::string Socket::CONNECT_SERVER_OK = "16";
const int Socket::MAX_MESSAGE_SIZE = 256;

// Opens socket connection that will be used between server and client
Socket::Socket(int port, bool reusePort, bool log) {
    this->log = log;
    struct sockaddr_in serv_addr;
    clilen = sizeof(struct sockaddr_in);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        printf("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    if (port != 0)
        serv_addr.sin_port = htons(port);
    else
        serv_addr.sin_port = 0;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    // TIMEOUT
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }

    if (reusePort)
        setReusePort();

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
        printf("ERROR on binding %d", port);

    connected = false;
}

// Close connection through socket
void Socket::closeSocket() {
    close(sockfd);
}

std::string Socket::getTime(){
    auto receiveTime = std::chrono::system_clock::now();
    std::time_t rec_time = std::chrono::system_clock::to_time_t(receiveTime);
    std::stringstream bufferTime;
    bufferTime << std::put_time(std::localtime(&rec_time), "%d/%b/%Y-%H:%M:%S");
    return bufferTime.str();
}

// Listens to message being received through the bind port if the connection is working
std::string Socket::listen() {
    int n;
    char buf[MAX_MESSAGE_SIZE];

    if (connected)
        n = recvfrom(sockfd, buf, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)NULL, &clilen);
    else
        n = recvfrom(sockfd, buf, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&oth_addr, &clilen);
    // Returns an empty message in case of a receive timeout or fail
    if (n <= 0) {
        return "";
    }

    if (log)
        printf("Received a datagram (at %s): %s\n", (char*)getTime().c_str(), buf);
    return std::string(buf);
}

// Sends a message through the bind port without an address defined
void Socket::send(const std::string &message) {
    int n;
    const char *messageC = message.c_str();
    if (connected)
        n = sendto(sockfd, messageC, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)NULL, sizeof(struct sockaddr));
    else
        n = sendto(sockfd, messageC, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&oth_addr, sizeof(struct sockaddr));
    // Returns an error if the send fails
    if (n < 0)
        printf("ERROR on sendto");
    else if (log)
        printf("Sent a datagram (at %s): %s\n", (char*)getTime().c_str(), messageC);
}

// Sends a message through the bind port to a given address
void Socket::send(const std::string &message, struct sockaddr_in addr) {
    int n;
    const char *messageC = message.c_str();
    n = sendto(sockfd, messageC, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    // Returns an error if send fails
    if (n < 0)
        printf("ERROR on sendto");
    else if (log)
        printf("Sent a datagram (at %s): %s\n", (char*)getTime().c_str(), messageC);
}

// Connect to the socket if the connection is working properly
void Socket::setConnect() {
    if (connect(sockfd, (struct sockaddr *)&oth_addr, sizeof(oth_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
    }
    connected = true;
}

// Sets a reusable port
void Socket::setReusePort() {
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEPORT) failed");
}

// Disable the connection logs from server or client
void Socket::disableLog() {
    log = false;
}

// Gets the destination socket address
struct sockaddr_in Socket::getoth_addr() {
    return oth_addr;
}

// Sets the destination address based on the host and the bind port, if it exists
void Socket::setoth_addr(char *hostname, int port) {
    struct hostent *server = gethostbyname(hostname);
    struct sockaddr_in new_addr;
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    new_addr.sin_family = AF_INET;
    new_addr.sin_port = htons(port);
    new_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(new_addr.sin_zero), 8);
    oth_addr = new_addr;
}


struct sockaddr_in Socket::create_addr(char *hostname, int port) {
    struct hostent *server = gethostbyname(hostname);
    struct sockaddr_in new_addr;
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    new_addr.sin_family = AF_INET;
    new_addr.sin_port = htons(port);
    new_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(new_addr.sin_zero), 8);
    return new_addr;
}

// Sets the destination address to a new one
void Socket::setoth_addr(struct sockaddr_in new_addr) {
    oth_addr = new_addr;
}

std::string Socket::get_addr_string(struct sockaddr_in addr) {
    char temp[256];
    getnameinfo((struct sockaddr *)&addr, sizeof addr, temp, 256, 0, 0, 0);
    return std::string(temp);
}

std::string Socket::get_addr_port_string(struct sockaddr_in addr) {
    char temp[256];
    getnameinfo((struct sockaddr *)&addr, sizeof addr, temp, 256, 0, 0, 0);
    return std::string(temp) + " " + std::to_string(ntohs(addr.sin_port));
}

// Splits a message in lines, reading from a stream object
std::vector<std::string> Socket::splitMessage(const std::string &message) {
    std::vector<std::string> spMessage;
    std::stringstream stream(message);
    std::string line;
    while (stream >> line) {
        spMessage.push_back(line);
    }
    return spMessage;
}

// Splits up a message up to a certain number of times
std::vector<std::string> Socket::splitUpToMessage(const std::string &message, int n) {
    std::vector<std::string> spMessage;
    std::stringstream stream(message);
    std::string line;
    int i = 1;
    while (i < n && stream >> line) {
        spMessage.push_back(line);
        ++i;
    }
    if (getline(stream, line)) {
        line.erase(line.find_last_not_of(" \t") + 1);
        line.erase(0, line.find_first_not_of(" \t"));
        spMessage.push_back(line);
    }
    return spMessage;
}

// Returns only the first line of the stream, which should contain the type of message
std::string Socket::getTypeMessage(const std::string &message) {
    std::stringstream stream(message);
    std::string line;
    stream >> line;
    return line;
}