
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>
#include <iostream>

void NotificationManager::listen(){
    while(1){
        // 1. Listen for notifications
        std::string input = sock.listen();

        if (input == "")
            continue;

        std::vector<std::string> spMessage = sock.splitUpToMessage(input, 5);
        std::string type = spMessage[0];

        // 2. if is a valid notification use interface.updateNotifications(notification)
        if (type == sock.NOTIFICATION){
            std::string notification = "@" + spMessage[2] + " " + spMessage[3] + "\n " + spMessage[4];
            interface.updateNotifications(stoi(spMessage[1]), notification);
    } else if (type == sock.ACK) { // verify if message is just an ACK from server
            if (spMessage[1] == "SEND") {
                std::cout << "Server received message \"";
                for (int i = 3; i < spMessage.size(); i++) 
                    std::cout << spMessage[i] + ((i < (spMessage.size() - 1)) ? " " : "");
                std::cout << "\" at " << spMessage[2] << std::endl;
            } else if (spMessage[1] == "FOLLOW") {
                if (spMessage[2] == "1") 
                    std::cout << "Profile \'" << spMessage[3] << "\' followed." << std::endl;
                else if (spMessage[2] == "0")
                    std::cout << "Profile \'" << spMessage[3] << "\' not found in server." << std::endl;
                else if (spMessage[2] == "2") 
                    std::cout << "You already follow profile \'" << spMessage[3] << "\'." << std::endl;
                else 
                    std::cout << "ERROR " << input << std::endl; 
            }
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}