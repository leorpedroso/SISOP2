
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
        } else if (type == sock.ACK) { 
            // 3. if message is an ACK from operations SEND or FOLLOW
            spMessage = sock.splitUpToMessage(spMessage[1], 3);
            
            if (spMessage[0] == "SEND") {
                // 3.1. if ACK is from SEND, print timestamp and message that the server received
                std::cout << "Server received message \"";
                std::cout << spMessage[2] << "\" at " << spMessage[1] << std::endl;
            } else if (spMessage[0] == "FOLLOW") {
                // 3.2 if ACk is from FOLLOW, print feedback from operation and user involved
                if (spMessage[1] == "1") 
                    std::cout << "Profile \'" << spMessage[2] << "\' followed." << std::endl;
                else if (spMessage[1] == "0")
                    std::cout << "Profile \'" << spMessage[2] << "\' not found in server." << std::endl;
                else if (spMessage[1] == "2") 
                    std::cout << "You already follow profile \'" << spMessage[2] << "\'." << std::endl;
                else 
                    std::cout << "ERROR " << input << std::endl; 
            }
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}