#pragma once

#include <string>
#include "interface.hpp"
#include "../common/socket.hpp"

class NotificationManager{
    private:
        std::string profile; // profile name
        Socket sock;
        Interface interface;

    public:
        NotificationManager(const std::string &profile, Socket sock, const Interface &interface) : 
                            profile(profile), sock(sock), interface(interface) {}

        // Listens for messages and shows them.             
        void listen();
};