#include "../../include/client/interface.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <signal.h>

void Interface::run(){
    std::string input;
    std::string command;
    std::string arg;

    while(true){
        getline(std::cin, input);
        if(std::cin.eof()){
            raise(SIGINT);
            break;
        }

        bool valid = parseString(input, command, arg);

        if (valid){
            if (command == "FOLLOW") {
                if (arg != profile){
                    follow(arg);
                    std::cout << "User followed." << std::endl;
                } else {
                    std::cout << "Can't follow yourself." << std::endl;
                }
            } else if (command == "SEND"){
                send(arg);
                std::cout << "Message sent." << std::endl;
            } else {
                std::cout << "Invalid command." << std::endl;    
            }
        } else {
            std::cout << "Invalid command." << std::endl;
        }
    }
}

bool Interface::parseString(std::string &input, std::string &command, std::string &arg){
    input.erase(input.find_last_not_of(" \t")+1); 
    input.erase(0, input.find_first_not_of(" \t"));

    std::stringstream ss(input);
    
    if(!(ss >> command)){
        command = "";
        return false;
    }
    if(!getline(ss, arg)){
        arg = "";
        return false;
    }

    arg.erase(arg.find_last_not_of(" \t")+1); 
    arg.erase(0, arg.find_first_not_of(" \t"));

    if(arg.size() > 128){
        arg = "";
        return false;
    }

    return true;
}

void Interface::follow(const std::string &name) {
    // send follow message.
    sock.send(sock.FOLLOW + " " + profile + " " + name);
}

void Interface::send(const std::string &message) {
    // send message to followers.
    sock.send(sock.SEND_NOTIFICATION + " " + profile + " " + message);
}

void Interface::updateNotifications(const std::string &notification){
    // print notifications on screen
    std::cout << notification << std::endl;
}