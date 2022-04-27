#include "../../include/server/notification.hpp"
#include "../../include/server/servermanager.hpp"


// Gets notification message
const std::string &Notification::getMessage() const {
    return message;
}      

// Gets the sender from the notification
const std::string &Notification::getSender() const {
    return sender;
}       

// Gets the time from the notification
const std::string &Notification::getTime() const {
    return time;
}

int Notification::getID(){
    return id;
}

std::shared_ptr<Counter> Notification::getCount(){
    return count;
}

// Increments read from the notification
void Notification::incrementRead() {
    ++read;
}

// Returns read from the notification
int Notification::getRead(){
    return read;
}