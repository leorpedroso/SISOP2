
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>
#include <iostream>

void NotificationManager::listen(){
    while(1){
        // 1. Listen for notifications
        std::string input = sock.listen();

        if (input == "")
            continue;

        std::vector<std::string> spMessage = sock.splitUpToMessage(input, 2);
        std::string type = spMessage[0];

        // 2. if is a valid notification use interface.updateNotifications(notification)
        if (type == sock.NOTIFICATION){
            spMessage = sock.splitUpToMessage(spMessage[1], 4);
            std::string notification = "@" + spMessage[1] + " " + spMessage[2] + "\n " + spMessage[3];
            interface.updateNotifications(stoi(spMessage[0]), notification);
        } else if (type == sock.ACK) { // verify if message is just an ACK from server
            spMessage = sock.splitUpToMessage(spMessage[1], 2);
            if (spMessage[0] == "SEND") {
                spMessage = sock.splitUpToMessage(spMessage[1], 2);
                std::cout << "Server received message \"";
                std::cout << spMessage[1] << "\" at " << spMessage[0] << std::endl;
            } else if (spMessage[0] == "FOLLOW") {
                if (spMessage[1] == "1") 
                    std::cout << "Profile \'" << spMessage[3] << "\' followed." << std::endl;
                else if (spMessage[1] == "0")
                    std::cout << "Profile \'" << spMessage[3] << "\' not found in server." << std::endl;
                else if (spMessage[1] == "2") 
                    std::cout << "You already follow profile \'" << spMessage[3] << "\'." << std::endl;
                else 
                    std::cout << "ERROR " << input << std::endl; 
            }
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}