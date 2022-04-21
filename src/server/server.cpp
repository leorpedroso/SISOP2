#include "../../include/server/server.hpp"

const std::string &Server::getName() const{
    return name;
}

const int &Server::getID() const{
    return ID;
}

const struct sockaddr_in &Server::getAddr() const{
    return addr;
}

