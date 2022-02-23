#include "../../include/server/notification.hpp"


const std::string &Notification::getMessage() const {
    return message;
}       

void Notification::incrementRead() {
    ++read;
}

int Notification::getRead(){
    return read;
}