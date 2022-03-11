#pragma once

#include <string>
#include "../common/socket.hpp"
#include <unordered_map>

class Interface{
    private:
        std::string profile;
        Socket sock;
        int notif_counter;
        std::unordered_map<int, std::string> notif_buffer;
        
        bool parseString(std::string &input, std::string &command, std::string &arg);
        void follow(const std::string &name);
        void send(const std::string &message);

    public:
        Interface(const std::string &profile, Socket sock) : profile(profile), sock(sock){}
        
        void run();
        void updateNotifications(int given_counter, const std::string &notification);
};