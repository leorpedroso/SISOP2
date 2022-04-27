#pragma once

#include <string>
#include "../common/socket.hpp"
#include <unordered_map>
#include<mutex>

class Interface{
    private:
        std::string profile; // profile name
        Socket sock;
        int notif_counter; // sequence number for notifications
        std::unordered_map<int, std::string> notif_buffer; // buffer for notifications
        
        // extracts command and args for a given user input
        bool parseString(std::string &input, std::string &command, std::string &arg);

        // sends follow message
        void follow(const std::string &name);

        // sends send message
        void send(const std::string &message);

    public:
        Interface(const std::string &profile, Socket sock) : profile(profile), sock(sock), notif_counter(0) {}

        void setNotifCounter(int val);
        void incrementNotifCounter();
        int getNotifCounter();
        
        // listens for user input and sends the required messages
        void run();

        // updates the notifications on screen
        void updateNotifications(int given_counter, const std::string &notification);
};