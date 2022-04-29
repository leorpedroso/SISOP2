#pragma once

#include<string>
#include<memory>
#include"counter.hpp"

class Notification{
    private:

        // message string, sender name and timestamp of notification
        std::string message;
        std::string sender;
        std::string time;

        // read counter for notification
        int read;

        // notification id
        int id;

        // notification counter
        std::shared_ptr<Counter> count;

    public:
        Notification(const std::string &message, const std::string &sender, const std::string &time): 
                                                    message(message), sender(sender), time(time), read(0),
                                                    count(nullptr), id(0) {}

        Notification(const std::string &message, const std::string &sender, const std::string &time, int id, std::shared_ptr<Counter> count): 
                                                    message(message), sender(sender), time(time), read(0),
                                                    count(count), id(id) {}
                        
        // getters for message, sender, time, count, id
        const std::string &getMessage() const;       
        const std::string &getSender() const;       
        const std::string &getTime() const;       
        std::shared_ptr<Counter> getCount();
        int getID();

        // functions for changing read value
        void incrementRead();
        int getRead();
};