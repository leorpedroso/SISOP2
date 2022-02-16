#include "../include/interface.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

void Interface::run(){
    std::string input;
    std::string command;
    std::string arg;

    while(true){
        getline(std::cin, input);
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

bool Interface::parseString(const std::string &input, std::string &command, std::string &arg){
    std::stringstream ss(input);
    std::vector<std::string> list;
    std::string word;

    while (ss >> word && list.size() <= 2) {
        list.push_back(word);
    }

    if (list.size() != 2){
        return false;
    }

    command = list[0];
    arg = list[1];

    return true;
}

void Interface::follow(const std::string &name) {
    // TODO
    // 1. send follow message.
}

void Interface::send(const std::string &message) {
    // TODO
    // 1. send message to followers.
}

void Interface::updateNotifications(const std::string &notification){
    std::cout << notification << std::endl;
}