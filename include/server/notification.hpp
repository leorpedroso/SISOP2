#pragma once

#include<string>

class Notification{
    private:

        // message string, sender name and timestamp of notification
        std::string message;
        std::string sender;
        std::string time;

        // read counter for notification
        int read;

    public:
        Notification(const std::string &message, const std::string &sender, const std::string &time): message(message), sender(sender), time(time), read(0) {}

        // getters for message, sender and time
        const std::string &getMessage() const;       
        const std::string &getSender() const;       
        const std::string &getTime() const;       

        // functions for changing read value
        void incrementRead();
        int getRead();
};