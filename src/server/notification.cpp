#include "../../include/server/notification.hpp"


const std::string &Notification::getMessage() const {
    return message;
}      

const std::string &Notification::getSender() const {
    return sender;
}       

void Notification::incrementRead() {
    ++read;
}

int Notification::getRead(){
    return read;
}