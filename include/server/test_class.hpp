#pragma once

#include <string>

class Test {
    public:
        Test();
        bool send(std::string msg);
        std::string receive(int timeout);
};