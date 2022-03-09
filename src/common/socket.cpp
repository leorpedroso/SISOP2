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

const std::string Socket::CONNECT = "0";
const std::string Socket::CONNECT_OK = "1";
const std::string Socket::CONNECT_NOT_OK = "2";
const std::string Socket::EXIT = "3";
const std::string Socket::FOLLOW = "4";
const std::string Socket::SEND_NOTIFICATION = "5";
const std::string Socket::NOTIFICATION = "6";
const int Socket::MAX_MESSAGE_SIZE = 256;

Socket::Socket(int port){
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
	 
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

    connected = false;
}

void Socket::closeSocket(){
    close(sockfd);
}

std::string Socket::listen(){
	int n;
	char buf[MAX_MESSAGE_SIZE];
	
    n = recvfrom(sockfd, buf, MAX_MESSAGE_SIZE, 0, (struct sockaddr *) &oth_addr, &clilen);
	if (n < 0) 
		printf("ERROR on recvfrom");
	printf("Received a datagram: %s\n", buf);

    return std::string(buf);
}

void Socket::send(const std::string &message){
	int n;
    const char *messageC = message.c_str();

	/* send to socket */
    n = sendto(sockfd, messageC, MAX_MESSAGE_SIZE, 0,(struct sockaddr *) &oth_addr, sizeof(struct sockaddr));

	if (n  < 0) 
		printf("ERROR on sendto");
}

void Socket::send(const std::string &message, struct sockaddr_in addr){
	int n;
    const char *messageC = message.c_str();

	/* send to socket */
    n = sendto(sockfd, messageC, MAX_MESSAGE_SIZE, 0,(struct sockaddr *) &addr, sizeof(struct sockaddr));

	if (n  < 0) 
		printf("ERROR on sendto");
}


struct sockaddr_in Socket::getoth_addr(){
    return oth_addr;
}

void Socket::setoth_addr(char *hostname, int port){
    struct hostent *server = gethostbyname(hostname);
    struct sockaddr_in new_addr;

	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
	
	new_addr.sin_family = AF_INET;     
	new_addr.sin_port = htons(port);    
	new_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(new_addr.sin_zero), 8);  

    oth_addr = new_addr;
}

void Socket::setoth_addr(struct sockaddr_in new_addr){
    oth_addr = new_addr;
}

std::vector<std::string> Socket::splitMessage(const std::string &message){
    std::vector<std::string> spMessage;
    std::stringstream stream(message);
    
    std::string line;
    while(stream >> line) {
        spMessage.push_back(line);
    }

    return spMessage;
}

std::vector<std::string> Socket::splitUpToMessage(const std::string &message, int n){
    std::vector<std::string> spMessage;
    std::stringstream stream(message);
    
    std::string line;
    int i = 1;
    while(i < n && stream >> line) {
        spMessage.push_back(line);
    }
    getline(stream, line);
    spMessage.push_back(line);

    return spMessage;
}

std::string Socket::getTypeMessage(const std::string &message){
    std::stringstream stream(message);
    std::string line;
    stream >> line;
    return line;
}