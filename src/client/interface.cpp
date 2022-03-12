#include "../../include/client/interface.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <signal.h>
#include <utility>

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
                } else {
                    std::cout << "Can't follow yourself." << std::endl;
                }
            } else if (command == "SEND"){
                send(arg);
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

void Interface::updateNotifications(int given_counter, const std::string &notification){
    // print notifications on screen
    if (notif_counter == given_counter) {
        notif_buffer.insert(std::make_pair(notif_counter, notification));
        std::unordered_map<int, std::string>::const_iterator next_notif = notif_buffer.find(notif_counter);
        
        while (next_notif != notif_buffer.end()) {
            std::cout << next_notif->second << std::endl;

            notif_buffer.erase(notif_counter);
            notif_counter++;
            next_notif = notif_buffer.find(notif_counter);
        }
    } else if (notif_counter < given_counter) {
        notif_buffer.insert(std::make_pair(given_counter, notification));
    }
}