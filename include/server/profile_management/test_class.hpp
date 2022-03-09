#pragma once

#include <string>

class Test {
    private:
        int counter;
        std::string clientName;

    public:
        Test(std::string clientName);
        bool send(std::string msg);
        std::string receive(int timeout);
};