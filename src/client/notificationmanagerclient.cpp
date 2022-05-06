
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>
#include <iostream>

void setAddr(struct sockaddr_in newAddr);
struct sockaddr_in getAddr();

// Receives a message from the server and process it
void NotificationManager::listen(){
    while(1){
        // Listens for notifications until it gets a valid input
        std::string input = sock.listen();
        if (input == "")
            continue;
        std::vector<std::string> spMessage = sock.splitUpToMessage(input, 2);
        // Ignore messages that don't come with type and message body separated. (probably corrupted messages)
        if(spMessage.size() < 2)
            continue;

        // If it is a notification type and a valid message, updates notification with the current message
        std::string type = spMessage[0];
        if (type == sock.NOTIFICATION){
            spMessage = sock.splitUpToMessage(spMessage[1], 4);
            if(spMessage.size() < 4)
                continue;
            std::string notification = "@" + spMessage[1] + " " + spMessage[2] + "\n \"" + spMessage[3] + "\"";
            interface.updateNotifications(stoi(spMessage[0]), notification);

        // If it is an ack from operations SEND or FOLLOW, check if the message is valid
        } else if (type == sock.ACK) { 
            spMessage = sock.splitUpToMessage(spMessage[1], 3);
            if(spMessage.size() < 3)
                continue;
            
            // If it is a SEND, print timestamp of arrival and message that the server received from this client
            if (spMessage[0] == "SEND") {
                std::cout << "Server received message \"";
                std::cout << spMessage[2] << "\" at " << spMessage[1] << std::endl;

            // If is is a FOLLOW, print feedback from operation and user involved, if it exists
            } else if (spMessage[0] == "FOLLOW") {
                if (spMessage[1] == "1") 
                    std::cout << "Profile \'" << spMessage[2] << "\' followed." << std::endl;
                else if (spMessage[1] == "0")
                    std::cout << "Profile \'" << spMessage[2] << "\' not found in server." << std::endl;
                else if (spMessage[1] == "2") 
                    std::cout << "You already follow profile \'" << spMessage[2] << "\'." << std::endl;
                else 
                    std::cout << "ERROR " << input << std::endl; 
            }
        } else if (type == sock.CONNECT_NOT_OK){
            std::cout << "Couldn't reconnect to server: " << input << std::endl;
            exit(1);
        } else if(type == sock.CONNECT_OK){
            std::vector<std::string> spMessage = sock.splitUpToMessage(input, 2);
            if(spMessage.size() < 2){
                std::cout << "ERROR " << input << std::endl;
                exit(1);
            }
            //DEBUG: std::cout << "NEW SERVER" << std::endl;
            // resets notification counter because a new session was started in a new server
            interface.setNotifCounter(0);
            // sets new addr
            setAddr(sock.getoth_addr());
            sock.send(sock.CONNECT_ACK + " ACK ACK", getAddr());

            // id = sock.splitUpToMessage(result, 2)[1];

        // Message of error in case the notification is invalid
        } else {
            std::cout << "ERROR " << input << std::endl;
        }
    }
}