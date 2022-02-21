#pragma once

#include<string>
#include<vector>
#include<mutex>
#include <queue>

class Notification{
    private:
        std::string message;
        int read;

    public:
        Notification(std::string message);

        const std::string &getMessage() const;       

        void incrementRead();
        int getRead();
};