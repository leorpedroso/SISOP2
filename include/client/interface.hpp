#pragma once

#include <string>

class Interface{
    private:
        std::string profile;
        std::string ip;
        int port;
        
        bool parseString(const std::string &input, std::string &command, std::string &arg);
        void follow(const std::string &name);
        void send(const std::string &message);

    public:
        Interface(const std::string &profile, const std::string &ip, int port) : profile(profile), ip(ip), port(port){}
        
        void run();
        void updateNotifications(const std::string &notification);
};