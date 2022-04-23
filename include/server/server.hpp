#pragma once

#include<string>
#include"../common/socket.hpp"
#include<queue>
#include"message.hpp"
#include<condition_variable>
#include<mutex>

class Server{
    private:
        std::string name;
        struct sockaddr_in addr;
        int ID;
        int port;

        std::mutex msgs_mtx; // mutex for update_msgs queue
        std::condition_variable msgs_cv; // condition variable that indicates that update_msgs is not empty

        std::queue<Message> msgs; // queue for update messages

    public:
        Server(struct sockaddr_in addr, int ID): addr(addr), name(Socket::get_addr_string(addr)), ID(ID), port(ntohs(addr.sin_port)) {}

        const std::string &getName() const;       
        const int &getID() const;      
        const int &getPort() const;      
        const struct sockaddr_in &getAddr() const;   

        void addMsg(Message msg);
        Message popMsg();
};