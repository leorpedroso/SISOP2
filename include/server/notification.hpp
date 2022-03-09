#pragma once

#include<string>

class Notification{
    private:
        std::string message;
        std::string sender;
        int read;

    public:
        Notification(const std::string &message, const std::string &sender): message(message), sender(sender) {}

        const std::string &getMessage() const;       
        const std::string &getSender() const;       

        void incrementRead();
        int getRead();
};