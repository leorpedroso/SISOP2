#pragma once

#include <string>

class Test {
    private:
    int counter;
    public:
        Test();
        bool send(std::string msg);
        std::string receive(int timeout);
};