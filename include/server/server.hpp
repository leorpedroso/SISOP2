#pragma once

#include<string>
#include"../common/socket.hpp"
#include<queue>
#include"message.hpp"
#include<memory>
#include"counter.hpp"
#include<condition_variable>
#include<mutex>

class Server{
    private:
        // server name, address, id and port
        std::string name;
        struct sockaddr_in addr;
        int ID;
        int port;

        std::mutex msgs_mtx; // mutex for update_msgs queue
        std::condition_variable msgs_cv; // condition variable that indicates that msgs is not empty

        // mutexes for name, addr, ID, port
        std::mutex name_mtx;
        std::mutex id_mtx;
        std::mutex port_mtx;
        std::mutex addr_mtx;

        std::queue<Message> msgs; // queue for update messages
        std::queue<std::shared_ptr<Counter>> msgs_counters;
        std::queue<std::shared_ptr<Counter>> msgs_counters_send;

    public:
        Server(struct sockaddr_in addr, int ID): addr(addr), name(Socket::get_addr_string(addr)), ID(ID), port(ntohs(addr.sin_port)) {}
        Server(int ID, const std::string &name, int port): ID(ID), name(name), port(port) {}

        // getters
        const std::string &getName();       
        const int &getID();      
        const int &getPort();      
        const struct sockaddr_in &getAddr();   

        // add message to server queue
        void addMsg(Message msg, std::shared_ptr<Counter> counter);
        // pop message from queue
        Message popMsg();
};