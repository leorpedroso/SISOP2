#pragma once

#include<string>

class Message{
    private:
        std::string type;
        std::string args;

    public:
        Message(const std::string &type, const std::string &args): type(type), args(args) {}

        // getters
        const std::string &getType() const;       
        const std::string &getArgs() const;       
};