#pragma once

#include<string>
#include"../common/socket.hpp"

class Server{
    private:
        std::string name;
        struct sockaddr_in addr;
        int ID;
        int port;

    public:
        Server(struct sockaddr_in addr, int ID): addr(addr), name(Socket::get_addr_string(addr)), ID(ID), port(ntohs(addr.sin_port)) {}

        const std::string &getName() const;       
        const int &getID() const;      
        const struct sockaddr_in &getAddr() const;   
};