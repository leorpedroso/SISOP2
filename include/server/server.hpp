#pragma once

#include<string>

class Server{
    private:
        std::string name;
        unsigned int ID;

    public:
        Server(const std::string &name, unsigned int ID): name(name), ID(ID){}

        const std::string &getName() const;       
        const unsigned int &getID() const;      
};