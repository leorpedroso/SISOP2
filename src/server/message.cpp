#include "../../include/server/message.hpp"

const std::string &Message::getType() const{
    return type;
}
const std::string &Message::getArgs() const{
    return args;
}