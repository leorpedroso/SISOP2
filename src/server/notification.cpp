#include "../../include/server/notification.hpp"

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

// Increments read from the notification
void Notification::incrementRead() {
    ++read;
}

// Returns read from the notification
int Notification::getRead(){
    return read;
}