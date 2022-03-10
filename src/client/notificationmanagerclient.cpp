
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>
#include <iostream>

void NotificationManager::listen(){
    while(1){
        // 1. Listen for notifications
        std::string input = sock.listen();

        if (input == "")
            continue;

        std::vector<std::string> spMessage = sock.splitUpToMessage(input, 4);
        std::string type = spMessage[0];

        // 2. if is a valid notification use interface.updateNotifications(notification)
        if (type == sock.NOTIFICATION){
            std::string notification = "@" + spMessage[1] + " " + spMessage[2] + "\n " + spMessage[3];
            interface.updateNotifications(notification);
        } else if (type == sock.ACK) { // verify if message is just an ACK from server
            if (spMessage[1] == "SEND") {
                std::cout << "Message sent." << std::endl;
            } else if (spMessage[1] == "FOLLOW") {
                if (spMessage[2] == "1") 
                    std::cout << "Profile followed." << std::endl;
                else if (spMessage[2] == "0")
                    std::cout << "Profile not found in server." << std::endl;
                else 
                    std::cout << "ERROR " << input << std::endl; 
            }
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}