#pragma once

#include<string>

class Notification{
    private:
        std::string message;
        std::string sender;
        std::string time;
        int read;

    public:
        Notification(const std::string &message, const std::string &sender, const std::string &time): message(message), sender(sender), time(time), read(0) {}

        const std::string &getMessage() const;       
        const std::string &getSender() const;       
        const std::string &getTime() const;       

        void incrementRead();
        int getRead();
};