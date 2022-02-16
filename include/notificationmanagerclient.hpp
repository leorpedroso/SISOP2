#pragma once

#include <string>
#include "interface.hpp"

class NotificationManager{
    private:
        std::string profile;
        std::string ip;
        int port;
        Interface interface;

    public:
        NotificationManager(const std::string &profile, const std::string &ip, int port, const Interface &interface) : profile(profile), ip(ip), port(port), interface(interface) {}
        void listen();
};