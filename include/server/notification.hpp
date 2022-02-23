#pragma once

#include<string>

class Notification{
    private:
        std::string message;
        int read;

    public:
        Notification(const std::string &message);

        const std::string &getMessage() const;       

        void incrementRead();
        int getRead();
};