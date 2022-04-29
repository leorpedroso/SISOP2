#pragma once

#include<string>
#include<mutex>
#include<memory>
#include"counter.hpp"

class Ack{
    private:
        // ack arguments
        std::string args;
        // Counter 
        std::shared_ptr<Counter> count;
        // ack id
        int id;

    public:
        Ack(const std::string &args, int id, std::shared_ptr<Counter> count): args(args), id(id), count(count){}

        // getters
        const std::string &getArgs() const;       
        std::shared_ptr<Counter> getCount();
};