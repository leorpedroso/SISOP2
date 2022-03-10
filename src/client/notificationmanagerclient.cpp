
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>
#include <iostream>

void NotificationManager::listen(){
    while(1){
        // 1. Listen for notifications
        std::string input = sock.listen();

        if (input == "")
            continue;

        std::vector<std::string> spMessage = sock.splitUpToMessage(input, 3);
        std::string type = spMessage[0];

        // 2. if is a valid notification use interface.updateNotifications(notification)
        if (type == sock.NOTIFICATION){
            std::string notification = "@" + spMessage[1] + " - " + spMessage[2];
            interface.updateNotifications(notification);
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}