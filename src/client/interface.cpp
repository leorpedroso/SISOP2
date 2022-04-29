#include "../../include/client/interface.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <signal.h>
#include <utility>

// setters/getter for socket addr with mutex
void setAddr(struct sockaddr_in newAddr);
struct sockaddr_in getAddr();

// mutex for notification counter
std::mutex notif_counter_mutex;

// Runs interface of client to treat input and output messages
void Interface::run(){
    std::string input;
    std::string command;
    std::string arg;

    while(true){
        // Reads the user input
        getline(std::cin, input);
        if(std::cin.eof()){
            raise(SIGINT);
            break;
        }
        bool valid = parseString(input, command, arg);

        // If it is a valid user input, treat the command
        if (valid){

            // If it is a FOLLOW, check if it the argument is not its own profile and executes the follow method (which sends a message to the server to follow the given profile)
            if (command == "FOLLOW") {
                if (arg != profile){
                    follow(arg);
                } else {
                    std::cout << "Can't follow yourself." << std::endl;
                }

            // If it is a SEND, sends the message
            } else if (command == "SEND"){
                send(arg);

            // Error message to the user in case of invalid command
            } else {
                std::cout << "Invalid command." << std::endl;    
            }
        } else {
            std::cout << "Invalid command." << std::endl;
        }
    }
}

// Gets the command and the arguments from splitting the input user string
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

// Sends to the server the FOLLOW command with current client's profile name and the profile name to be followed
void Interface::follow(const std::string &name) {
    sock.send(sock.FOLLOW + " " + profile + " " + name, getAddr());
}

// Sends the NOTIFICATION command to the server with the current client's profile and the message
// Sends a message to the server in order for it to reach all of its followers
void Interface::send(const std::string &message) {
    sock.send(sock.SEND_NOTIFICATION + " " + profile + " " + message, getAddr());
}

// Output for the user all its received notifications
void Interface::updateNotifications(int given_counter, const std::string &notification){
    int notif_counter_local = getNotifCounter();
    if (notif_counter_local == given_counter) {
        notif_buffer.insert(std::make_pair(notif_counter_local, notification));
        std::unordered_map<int, std::string>::const_iterator next_notif = notif_buffer.find(notif_counter_local);        
        while (next_notif != notif_buffer.end()) {
            std::cout << next_notif->second << std::endl;
            notif_buffer.erase(notif_counter_local);
            incrementNotifCounter();
            ++notif_counter_local;
            next_notif = notif_buffer.find(notif_counter_local);
        }
    } else {
        notif_buffer.insert(std::make_pair(given_counter, notification));
    }
}

void Interface::setNotifCounter(int val){
    std::unique_lock<std::mutex> mlock(notif_counter_mutex);
    notif_counter = val;
}

void Interface::incrementNotifCounter(){
    std::unique_lock<std::mutex> mlock(notif_counter_mutex);
    ++notif_counter;
}

int Interface::getNotifCounter(){
    std::unique_lock<std::mutex> mlock(notif_counter_mutex);
    return notif_counter; 
}