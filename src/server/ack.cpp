#include "../../include/server/ack.hpp"
#include "../../include/server/servermanager.hpp"

const std::string &Ack::getArgs() const{
    return args;
}

std::shared_ptr<Counter> Ack::getCount() {
    return count;
}