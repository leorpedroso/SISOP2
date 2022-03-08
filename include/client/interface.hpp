#pragma once

#include <string>
#include "../common/socket.hpp"

class Interface{
    private:
        std::string profile;
        Socket sock;
        
        bool parseString(const std::string &input, std::string &command, std::string &arg);
        void follow(const std::string &name);
        void send(const std::string &message);

    public:
        Interface(const std::string &profile, Socket sock) : profile(profile), sock(sock){}
        
        void run();
        void updateNotifications(const std::string &notification);
};